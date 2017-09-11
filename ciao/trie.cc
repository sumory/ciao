#include "ciao/trie.h"

namespace ciao {

Node* Trie::add_node(std::string pattern) {
    pattern = trim_path_spaces(pattern);
    auto search = pattern.find("//");
    if (search != std::string::npos) {
        CIAO_GODIE("`//` is not allowed: " + pattern);
    }

    std::string tmp_pattern = trim_prefix_slash(pattern);
    std::vector<std::string> segments = string_split(tmp_pattern, '/');

    Node* node = _insert_node(root, segments);
    if (node->pattern.empty()) {
        node->pattern = pattern;
    }

    return node;
}

Node* Trie::get_colon_node(Node* parent, std::string segment) {
    Node* child = parent->colon_child;

    if (child && child->regex.empty() != true) {
        bool matched = is_str_match_regex(segment, child->regex);
        if (!matched) {
            child = nullptr;
        }
    }

    return child;
}

// retry to fallback to lookup the colon nodes in `stack`
LookupResult Trie::fallback_lookup(FallbackStack& fallback_stack, std::vector<std::string> segments,
                                   std::unordered_map<std::string, std::string> params) {
    Matched matched;
    LookupResult r = std::make_pair(false, matched);
    if (fallback_stack.size() == 0) {
        return r;  // 返回<false, 空值>
    }

    std::pair<int, Node*> fallback = fallback_stack.back();
    fallback_stack.pop_back();

    int segment_index = fallback.first;  // index
    Node* parent = fallback.second;      // colon_node

    if (!(parent->name.empty())) {  // fallback to the colon node and fill param if matched
        matched.params[parent->name] = segments[segment_index];
    }

    mixin(params, matched.params);  // mixin params parsed before
    bool flag = true;
    for (size_t i = 0; i < segments.size(); i++) {
        std::string s = segments[i];
        if (i <= size_t(segment_index)) {  // mind: should use <= not <
            continue;
        }

        Node* node;
        Node* colon_node;
        bool is_same;
        std::tie(node, colon_node, is_same) = find_matched_child(parent, s);
        if (_ignore_case && !node) {
            std::tie(node, colon_node, is_same) = find_matched_child(parent, string_to_lower(s));
        }

        if (colon_node && !is_same) {
            // save colon node to fallback stack
            std::pair<int, Node*> tmp_fallback = std::make_pair(i, colon_node);
            fallback_stack.emplace_back(tmp_fallback);
        }

        if (!node) {       // both exact child and colon child is nil
            flag = false;  // should not set parent value
            break;
        }

        parent = node;
    }

    if (flag && parent->endpoint) {
        matched.node = parent;
        matched.pipeline = _get_pipeline(parent);
    }

    if (matched.node) {
        r.first = true;
    } else {
        r.first = false;
    }

    return r;
}

// find exactly mathed node and colon node
MatchedChild Trie::find_matched_child(Node* parent, std::string segment) {
    Node* child = parent->find_child(segment);
    Node* colon_node = get_colon_node(parent, segment);

    if (child) {
        if (colon_node) {
            MatchedChild m(child, colon_node, false);
            return m;
        } else {
            MatchedChild m(child, nullptr, false);
            return m;
        }
    } else {
        if (colon_node) {
            MatchedChild m(colon_node, colon_node, true);  //  后续不再压栈
            return m;
        } else {
            MatchedChild m(nullptr, nullptr, false);
            return m;
        }
    }
}

Matched Trie::match(std::string path) {
    if (path.empty()) {
        CIAO_GODIE("`path` should not be nil or empty");
    }

    path = slim_path(path);
    std::string first = path.substr(0, 1);
    if (std::strcmp("/", first.c_str()) != 0) {
        CIAO_GODIE("`path` is not start with prefix /: " + path);
    }

    if (first.empty()) {  // special case: regard "test.com" as "test.com/"
        first = "/";
    }

    Matched matched = _match(path);
    if (!matched.node && _strict_route) {
        std::string last = path.substr(path.length() - 1);
        if (std::strcmp("/", last.c_str()) == 0) {  // retry to find path without last slash
            matched = _match(path.substr(0, path.length() - 1));
        }
    }

    return matched;
}

Matched Trie::_match(std::string path) {
    std::vector<std::string> segments;
    segments = string_split(path, '/');
    segments.erase(segments.begin());  // remove first empty item: ""

    bool flag = true;  // whether to continue to find matched node or not
    Matched matched;
    matched.node = nullptr;  // important! explicitly
    Node* parent = root;
    FallbackStack fallback_stack;

    for (size_t i = 0; i < segments.size(); i++) {
        std::string s = segments[i];
        Node* node;
        Node* colon_node;
        bool is_same;
        std::tie(node, colon_node, is_same) = find_matched_child(parent, s);
        if (_ignore_case && !node) {
            std::tie(node, colon_node, is_same) = find_matched_child(parent, string_to_lower(s));
        }

        if (colon_node && !is_same) {
            fallback_stack.push_back(std::make_pair(static_cast<int>(i), colon_node));
        }

        if (!node) {       //  both exact child and colon child is nil
            flag = false;  // should not set parent value
            break;
        }

        parent = node;
        if (!(parent->name.empty())) {
            matched.params[parent->name] = s;
        }
    }

    if (flag && parent->endpoint) {
        matched.node = parent;
    }

    auto params = matched.params;  // init
    bool die = false;
    if (!matched.node) {
        int depth = 0;
        bool exit = false;

        while (!exit) {
            depth++;
            if (depth > _max_fallback_depth) {
                die = true;
                break;
            }

            LookupResult lr = fallback_lookup(fallback_stack, segments, params);
            exit = lr.first;
            if (exit) {  // found
                matched = lr.second;
                break;
            }

            if (fallback_stack.size() == 0) {
                break;
            }
        }
    }

    if (die) {
        std::ostringstream oss;
        oss << "fallback lookup reaches the limit: " << _max_fallback_depth;
        CIAO_GODIE(oss.str());
    }

    matched.params = params;
    if (matched.node) {
        matched.pipeline = _get_pipeline(matched.node);
    }

    return matched;
}

bool Trie::_check_segment(const std::string& segment) {
    bool matched = std::regex_match(segment, _uri_segment_pattern);
    return matched;
}

ValidColonChild Trie::_check_colon_child(Node* node, Node* colon_child) {
    ValidColonChild result(false, nullptr);
    if (!node || !colon_child) {
        return result;
    }

    int name_equal = std::strcmp(node->name.c_str(), colon_child->name.c_str());
    int regex_equal = std::strcmp(node->regex.c_str(), colon_child->regex.c_str());
    if (name_equal != 0 || regex_equal != 0) {
        result.second = colon_child;
        return result;
    }

    result.first = true;  // could be added
    return result;
}

Node* Trie::_get_or_new_node(Node* parent, std::string frag) {
    if (frag.empty() || std::strcmp("/", frag.c_str()) == 0) {
        frag = "";
    }

    if (_ignore_case) {
        frag = string_to_lower(frag);
    }

    Node* node = parent->find_child(frag);
    if (node) {
        return node;
    }

    _counter++;
    node = new Node(false, _counter);
    node->parent = parent;

    if (std::strcmp("", frag.c_str()) == 0) {
        NodeHolder node_pack;
        node_pack.key = frag;
        node_pack.val = node;
        parent->children.emplace_back(node_pack);
    } else {
        std::string first = frag.substr(0, 1);
        if (std::strcmp(":", first.c_str()) == 0) {
            std::string name = frag.substr(1);
            std::string trailing = name.substr(name.size() - 1);

            if (std::strcmp(")", trailing.c_str()) == 0) {
                auto search = name.find_first_of("(");
                if (search != std::string::npos) {
                    int index = static_cast<int>(search);
                    std::string regex = name.substr(index + 1, name.size() - index - 2);
                    if (!regex.empty()) {
                        name = name.substr(0, index);
                        node->regex = regex;
                    } else {
                        CIAO_GODIE("invalid pattern[1]: " + frag);
                    }
                }
            }

            bool is_name_valid = _check_segment(name);
            if (!is_name_valid) {
                std::ostringstream oss;
                oss << "invalid pattern[2], illegal path:" << name << ", valid pattern is ["
                    << _segment_pattern << "]";
                CIAO_GODIE(oss.str());
            }

            node->name = name;
            Node* colon_child = parent->colon_child;
            if (colon_child) {
                auto valid_colon_child = _check_colon_child(node, colon_child);
                if (!valid_colon_child.first) {
                    std::ostringstream oss;
                    oss << "invalid pattern[3]: [" << name << "] conflict with ["
                        << valid_colon_child.second->name << "]";
                    CIAO_GODIE(oss.str());
                } else {
                    // TODO: check here!
                    delete node;
                    return colon_child;
                }
            }

            parent->colon_child = node;
        } else {
            bool is_name_valid = _check_segment(frag);
            if (!is_name_valid) {
                std::ostringstream oss;
                oss << "invalid pattern[4], " << frag << ", valid pattern is [" << _segment_pattern
                    << "]";
                CIAO_GODIE(oss.str());
            }

            NodeHolder node_pack;
            node_pack.key = frag;
            node_pack.val = node;
            parent->children.emplace_back(node_pack);
        }
    }

    return node;
}

Node* Trie::_insert_node(Node* parent, std::vector<std::string> frags) {
    std::string frag = frags.front();
    Node* child = _get_or_new_node(parent, frag);

    if (frags.size() >= 1) {
        frags.erase(frags.begin());
    }

    if (frags.size() == 0) {
        child->endpoint = true;
        return child;
    }

    return _insert_node(child, frags);
}

std::vector<Node*> Trie::_get_pipeline(Node* node) {
    std::vector<Node*> pipeline;
    if (!node) return pipeline;

    std::vector<Node*> tmp;
    Node* origin_node = node;
    tmp.push_back(origin_node);
    while (node->parent) {
        tmp.push_back(node->parent);
        node = node->parent;
    }

    for (auto it = tmp.rbegin(); it != tmp.rend(); it++) {
        pipeline.emplace_back(*it);
    }

    return pipeline;
}

void Trie::set_segment_pattern(std::string v) {
    _segment_pattern = v;
    _uri_segment_pattern = std::regex(v);
}

}  // namespace ciao
