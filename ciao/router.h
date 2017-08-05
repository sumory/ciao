#pragma once
#include "ciao/define.h"
#include "ciao/node.h"
#include "ciao/request.h"
#include "ciao/response.h"
#include "ciao/trie.h"
#include "ciao/utils.h"

namespace ciao {

std::vector<ActionHolder&> compose_func(Matched& matched, const std::string& method) {
    std::vector<ActionHolder&> stack;
    if (!matched) {
        return stack;
    }

    Node* exact_node = matched.node;
    if (!exact_node) {
        return stack;
    }

    std::vector<Node*> pipeline = matched.pipeline;
    if (pipeline.size() == 0) {
        return stack;
    }

    for (size_t i = 0; i < pipeline.size(); i++) {
        Node* p = pipeline[i];
        std::vector<ActionHodler> middlewares = p.middlewares;
        for (ActionHolder& ah : middlewares) {
            stack.push_back(ah);
        }

        std::unordered_map<std::string, std::vector<ActionHolder>> handlers = p.handlers;

        // TODO: id must be unique!
        if (p->id == exact_node->id && handlers.find(method) != handlers.end()) {
            std::vector<ActionHolder>& hs = handlers[method];
            for (ActionHolder& ah : hs) {
                stack.push_back(ah);
            }
        }
    }

    return stack;
}

std::vector<ActionHolder&> compose_error_handler(Node* node) {
    std::vector<ActionHolder&> stack;
    if (!node) {
        return stack;
    }

    while (node) {
        for (ActionHolder& ah : node.error_middlewares) {
            stack.push_back(ah);
        }
        node = node->parent;
    }
    return stack;
}

class Router {
 public:
    std::string name;
    Trie trie;

 public:
    Router(std::string n, bool ignore_case = true, bool strict_route = true,
           std::string segment_pattern = "[A-Za-z0-9._~\\-]+", int max_uri_segments = 100,
           int max_fallback_depth = 100)
        : name(n),
          trie(ignore_case, strict_route, segment_pattern, max_uri_segments, max_fallback_depth) {
        if (n.empty()) {
            name = "router-" + gen_random_str();
        }
    }

    //  Router() returns `Middleware`, so router itself could be regarded as Middleware.
    Middleware operator()() { return call(); }

    Middleware call() {
        Middleware m = [this](Request req, Response res, Next next) {
            return handle(req, res, next);
        };
        return m;
    }

    void handle(Request req, Response res, Next& done) {
        std::string path = req.path;
        std::string method = string_to_lower(req.method);

        std::vector<ActionHolder&> stack;  // an array actually, not a `stack`
        Matched matched = trie.match(path);
        Node* matched_node = matched.node;

        if (method.empty() || !matched_node) {
            error_handle("404! not found.", req, res, trie.root, done);
            return;
        }
    }

    // dispatch an error
    void error_handle(std::string err_msg, Request& req, Response& res, Node* node, Next& done) {
        std::vector<ActionHolder&> stack;

        return;
    }

    Router* set_ignore_case(std::string v) {
        trie._ignore_case = v;
        return this;
    }

    Router* set_strict_route(std::string v) {
        trie._strict_route = v;
        return this;
    }

    Router* set_segment_pattern(std::string v) {
        trie.set_segment_pattern(v);
        return this;
    }

    Router* set_max_uri_segments(int v) {
        trie._max_uri_segments = v;
        return this;
    }

    Router* set_max_fallback_depth(int v) {
        trie._max_fallback_depths = v;
        return this;
    }
};

}  // namespace ciao
