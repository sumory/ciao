#pragma once
#include "ciao/define.h"
#include "ciao/utils.h"

namespace ciao {

class Node;

struct NodeHolder {
    std::string key;
    Node* val;
};

struct ActionHolder {
    std::string id;
    Node* node;
    std::string action_type;
    Middleware func;

    ActionHolder(const std::string& i, Node* n, std::string at, Middleware m)
        : id(i), node(n), action_type(at), func(m) {}
};

struct ErrorActionHolder {
    std::string id;
    Node* node;
    std::string action_type;
    ErrorMiddleware func;

    ErrorActionHolder(const std::string& i, Node* n, std::string at, ErrorMiddleware m)
        : id(i), node(n), action_type(at), func(m) {}
};

struct Matched {
    Node* node;
    std::unordered_map<std::string, std::string> params;
    std::vector<Node*> pipeline;
};

class Node {
 private:
    Node* _use(Middleware& m);
    Node* _error_use(ErrorMiddleware& m);
    Node* _handle(const std::string& method, Middleware& m);

 public:
    std::string id;
    bool is_root;
    std::string name;
    std::unordered_set<std::string> allow;
    std::string pattern;
    bool endpoint;
    Node* parent;
    Node* colon_parent;
    std::vector<NodeHolder> children;
    Node* colon_child;
    std::unordered_map<std::string, std::vector<ActionHolder>> handlers;
    std::vector<ActionHolder> middlewares;
    std::vector<ErrorActionHolder> error_middlewares;
    std::string regex;

 public:
    explicit Node(bool root = false) {
        is_root = root;
        parent = nullptr;
        colon_parent = nullptr;
        colon_child = nullptr;
        endpoint = false;
        if (is_root) {
            id = "root_" + gen_random_str();
        } else {
            id = "node_" + gen_random_str();
        }
    }

    Node(bool root, int unique_id) {
        is_root = root;
        parent = nullptr;
        colon_parent = nullptr;
        colon_child = nullptr;
        endpoint = false;
        if (is_root) {
            id = "root_" + std::to_string(unique_id);
        } else {
            std::ostringstream oss;
            oss << "node_" << gen_random_str() << "_" << unique_id;
            id = oss.str();
        }
    }

    ~Node() {
        // TODO: check
        // std::cout << "release node: " << id << std::endl;
        delete colon_child;
        for (auto& nh : children) {
            delete nh.val;
        }
    }

    bool check_method(const std::string& method);
    Node* find_child(const std::string& key);
    bool find_handler(const std::string& method);
    Node* use(Middleware m);
    Node* use(std::vector<Middleware> ms);
    Node* error_use(ErrorMiddleware m);
    Node* error_use(std::vector<ErrorMiddleware> ms);
    Node* handle(const std::string& method, Middleware m);
    Node* handle(const std::string& method, std::vector<Middleware> ms);
};

typedef std::shared_ptr<Node> NodePtr;

}  // namespace ciao
