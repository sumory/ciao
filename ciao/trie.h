#include "ciao/define.h"
#include "ciao/node.h"
#include "ciao/utils.h"

namespace ciao {

typedef std::vector<std::pair<int, Node*>> FallbackStack;
typedef std::tuple<Node*, Node*, bool> MatchedChild;
typedef std::pair<bool, Node*> ValidColonChild;
typedef std::pair<bool, Matched> LookupResult;

class Trie {
 public:
    // bettor to hide these properties.
    bool _ignore_case;
    bool _strict_route;
    std::string _segment_pattern;
    int _max_uri_segments;
    int _max_fallback_depth;
    // the root Node of trie
    Node* root;
    int _counter;

 private:
    std::regex _uri_segment_pattern;

 public:
    Trie(bool ignore_case = true, bool strict_route = true,
         std::string segment_pattern = "[A-Za-z0-9._~\\-]+", int max_uri_segments = 100,
         int max_fallback_depth = 100)
        : _ignore_case(ignore_case),
          _strict_route(strict_route),
          _segment_pattern(segment_pattern),
          _max_uri_segments(max_uri_segments),
          _max_fallback_depth(max_fallback_depth) {
        _uri_segment_pattern = std::regex(segment_pattern);
        root = new Node(true);
        _counter = 0;
    }
    ~Trie() {
        // TODO: delete recursively
        if (root) {
            delete root;
        }
    }
    Node* add_node(std::string pattern);
    Node* get_colon_node(Node* parent, std::string segment);
    LookupResult fallback_lookup(FallbackStack& fallback_stack, std::vector<std::string> segments,
                                 std::unordered_map<std::string, std::string> params);
    MatchedChild find_matched_child(Node* parent, std::string segment);
    Matched match(std::string path);

    bool _check_segment(const std::string& segment);
    ValidColonChild _check_colon_child(Node* node, Node* colon_child);
    Node* _get_or_new_node(Node* parent, std::string frag);
    Node* _insert_node(Node* parent, std::vector<std::string> frags);
    std::vector<Node*> _get_pipeline(Node* node);
    Matched _match(std::string path);
    void set_segment_pattern(std::string v);
};

}  // namespace ciao
