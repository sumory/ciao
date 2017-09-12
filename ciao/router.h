#pragma once
#include "ciao/define.h"
#include "ciao/group.h"
#include "ciao/node.h"
#include "ciao/request.h"
#include "ciao/response.h"
#include "ciao/trie.h"
#include "ciao/utils.h"

namespace ciao {

class Router {
 public:
    std::string name;
    Trie trie;

 public:
    Router(std::string n = "", bool ignore_case = true, bool strict_route = true,
           std::string segment_pattern = "[A-Za-z0-9\\._~\\-]+", int max_uri_segments = 100,
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
        return [this](Request& req, Response& res, Next& next) {
            handle(req, res, next);
            return;
        };
    }

    void handle(Request& req, Response& res, Next& done) {
        std::string path = req.path;
        std::string method = string_to_lower(req.method);

        Matched matched = trie.match(path);
        Node* matched_node = matched.node;
        if (method.empty() || !matched_node) {
            res.status(404);
            error_handle("404! not found.", req, res, trie.root, done);
            return;
        }

        std::vector<ActionHolder> matched_handlers;
        auto it = matched_node->handlers.find(method);
        if (it != matched_node->handlers.end()) {
            matched_handlers = it->second;
        }

        if (matched_handlers.size() == 0) {
            res.status(404);
            error_handle("Oh! no handler to process method:" + method, req, res, trie.root, done);
            return;
        }

        std::vector<ActionHolder> stack;  // an array actually, not a real `stack`
        stack = compose_func(matched, method);
        if (stack.size() == 0) {
            res.status(404);
            error_handle("Oh! no handlers found.", req, res, trie.root, done);
            return;
        }

        req.set_found(true);
        std::unordered_map<std::string, std::string> parsed_params = matched.params;
        req.params = parsed_params;

        size_t idx = 0;
        size_t stack_len = stack.size();
        Next next = [&](const std::string& err) {
            CIAO_DEBUG("next, idx:" + std::to_string(idx) +
                       " stack_len:" + std::to_string(stack_len) + " err:" + err);

            if (!err.empty()) {
                error_handle(err, req, res, stack[idx].node, done);
                return;
            }

            if (idx > stack_len) {
                done(err);  // err is nil or not
                return;
            }

            ActionHolder& handler = stack[idx];
            idx++;  // should not place it at the end!

            bool result = false;
            std::string err_msg;
            try {
                handler.func(req, res, next);
                req.params = mixin(parsed_params, req.params);
                result = true;
            } catch (const std::exception& e) {
                err_msg = "[HANDLER ERROR]" + std::string(e.what());
                CIAO_ERROR(err_msg);
                result = false;
            } catch (...) {
                err_msg = "[HANDLER ERROR] unknown process request!";
                CIAO_ERROR(err_msg);
                result = false;
            }

            if (!result) {
                error_handle(err_msg, req, res, handler.node, done);
                return;
            }
        };

        next("");
    }

    // dispatch an error
    void error_handle(std::string err_msg, Request& req, Response& res, Node* node, Next& done) {
        std::vector<ErrorActionHolder> stack = compose_error_handler(node);
        if (stack.size() == 0) {
            done(err_msg);
            return;
        }

        size_t idx = 0;
        size_t stack_len = stack.size();
        Next next = [&](const std::string& err) {
            CIAO_DEBUG("err_next, idx:" + std::to_string(idx) +
                       " stack_len:" + std::to_string(stack_len) + " err:" + err);

            if (idx >= stack_len) {
                done(err);  // err is nil or not
                return;
            }

            ErrorActionHolder& error_handler = stack[idx];
            idx++;

            bool result = false;
            std::string _err;
            try {
                error_handler.func(err_msg, req, res, next);
                result = true;
            } catch (const std::exception& e) {
                _err = "[ERROR_HANDLER ERROR]" + std::string(e.what());
                CIAO_ERROR(_err);
                result = false;
            } catch (...) {
                _err = "[ERROR_HANDLER ERROR] unknown process request!";
                CIAO_ERROR(_err);
                result = false;
            }

            if (!result) {
                done(_err);
                return;
            }
        };

        next(err_msg);
    }

    Router* use(const std::string& path, Middleware& m) {
        Node* node;
        if (path.empty()) {
            node = trie.root;
        } else {
            node = trie.add_node(path);
        }

        node->use(m);
        return this;
    }

    Router* use(const std::string& path, std::vector<Middleware> ms) {
        Node* node;
        if (path.empty()) {
            node = trie.root;
        } else {
            node = trie.add_node(path);
        }

        node->use(ms);
        return this;
    }

    Router* error_use(const std::string& path, ErrorMiddleware& m) {
        Node* node;
        if (path.empty()) {
            node = trie.root;
        } else {
            node = trie.add_node(path);
        }

        node->error_use(m);
        return this;
    }

    Router* error_use(const std::string& path, std::vector<ErrorMiddleware> ms) {
        Node* node;
        if (path.empty()) {
            node = trie.root;
        } else {
            node = trie.add_node(path);
        }

        node->error_use(ms);
        return this;
    }

    // TODO: WIP
    Router* use(const std::string& path, Group& g) { return this; }

    Router* app_route(const std::string& method, const std::string& path, Middleware& m) {
        Node* node = trie.add_node(path);
        node->handle(method, m);
        return this;
    }

    Router* app_route(const std::string& method, const std::string& path,
                      std::vector<Middleware> ms) {
        Node* node = trie.add_node(path);
        node->handle(method, ms);
        return this;
    }

    std::vector<ActionHolder> compose_func(Matched& matched, const std::string& method) {
        std::vector<ActionHolder> stack;

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
            std::vector<ActionHolder> middlewares = p->middlewares;
            for (ActionHolder& ah : middlewares) {
                stack.push_back(ah);
            }

            std::unordered_map<std::string, std::vector<ActionHolder>> handlers = p->handlers;

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

    std::vector<ErrorActionHolder> compose_error_handler(Node* node) {
        std::vector<ErrorActionHolder> stack;
        if (!node) {
            return stack;
        }

        while (node) {
            for (ErrorActionHolder& ah : node->error_middlewares) {
                stack.push_back(ah);
            }
            node = node->parent;
        }
        return stack;
    }

    // basic methods start...
    Router* _init_basic_method(const std::string& method, const std::string& path, Middleware m) {
        Node* node = trie.add_node(path);
        node->handle(method, m);
        return this;
    }
    Router* _init_basic_method(const std::string& method, const std::string& path,
                               std::vector<Middleware> ms) {
        Node* node = trie.add_node(path);
        node->handle(method, ms);
        return this;
    }
    Router* get(const std::string& path, Middleware m) {
        return _init_basic_method("get", path, m);
    }
    Router* get(const std::string& path, std::vector<Middleware> ms) {
        return _init_basic_method("get", path, ms);
    }
    Router* post(const std::string& path, Middleware m) {
        return _init_basic_method("post", path, m);
    }
    Router* post(const std::string& path, std::vector<Middleware> ms) {
        return _init_basic_method("post", path, ms);
    }
    Router* del(const std::string& path, Middleware m) {
        return _init_basic_method("delete", path, m);
    }
    Router* del(const std::string& path, std::vector<Middleware> ms) {
        return _init_basic_method("delete", path, ms);
    }
    Router* put(const std::string& path, Middleware m) {
        return _init_basic_method("put", path, m);
    }
    Router* put(const std::string& path, std::vector<Middleware> ms) {
        return _init_basic_method("put", path, ms);
    }
    Router* patch(const std::string& path, Middleware m) {
        return _init_basic_method("patch", path, m);
    }
    Router* patch(const std::string& path, std::vector<Middleware> ms) {
        return _init_basic_method("patch", path, ms);
    }
    // basic methods end.

    // conf getter/setter
    Router* set_ignore_case(bool v) {
        trie._ignore_case = v;
        return this;
    }
    Router* set_strict_route(bool v) {
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
        trie._max_fallback_depth = v;
        return this;
    }
};

}  // namespace ciao
