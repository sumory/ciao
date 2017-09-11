#include "ciao/node.h"

namespace ciao {

bool Node::check_method(const std::string& method) {
    if (method.empty()) return false;
    if (supported_http_methods.find(method) != supported_http_methods.end()) {
        return true;
    }
    return false;
}

Node* Node::find_child(const std::string& key) {
    for (auto& n : children) {
        if (std::strcmp(key.c_str(), n.key.c_str()) == 0) {
            return n.val;
        }
    }
    return nullptr;
}

bool Node::find_handler(const std::string& method) {
    std::string s = string_to_lower(method);
    auto found = handlers.find(s);
    if (found != handlers.end() && found->second.size() > 0) {
        return true;
    }
    return false;
}

Node* Node::_use(Middleware& m) {
    std::string action_id = gen_random_str();
    ActionHolder action(action_id, this, "middleware", m);
    middlewares.emplace_back(action);
    return this;
}

Node* Node::use(Middleware m) { return _use(m); }

Node* Node::use(std::vector<Middleware> ms) {
    for (auto& m : ms) {
        _use(m);
    }
    return this;
}

Node* Node::_error_use(ErrorMiddleware& m) {
    std::string action_id = gen_random_str();
    ErrorActionHolder action(action_id, this, "error_middleware", m);
    error_middlewares.emplace_back(action);
    return this;
}

Node* Node::error_use(ErrorMiddleware m) { return _error_use(m); }

Node* Node::error_use(std::vector<ErrorMiddleware> ms) {
    for (auto& m : ms) {
        _error_use(m);
    }
    return this;
}

Node* Node::_handle(const std::string& method, Middleware& m) { return nullptr; }

Node* Node::handle(const std::string& method, Middleware m) {
    std::string s = string_to_lower(method);
    std::ostringstream oss;
    if (!check_method(method)) {
        oss << "error method:" << method;
        CIAO_GODIE(oss.str());  // should not register unsupported method->handler.
    }

    if (find_handler(method)) {
        oss << "[" << pattern << "] [" << method << "] handler exists yet!!!";
        CIAO_GODIE(oss.str());
    }

    std::string action_id = gen_random_str();
    ActionHolder action(action_id, this, "handler", m);

    handlers[method].emplace_back(action);
    allow.insert(method);
    return this;
}

Node* Node::handle(const std::string& method, std::vector<Middleware> ms) {
    std::string s = string_to_lower(method);
    std::ostringstream oss;
    if (!check_method(method)) {
        oss << "error method:" << method;
        CIAO_GODIE(oss.str());  // should not register unsupported method->handler.
    }

    if (find_handler(method)) {
        oss << "[" << pattern << "] [" << method << "] handler exists yet!!!";
        CIAO_GODIE(oss.str());
    }

    for (auto m : ms) {
        std::string action_id = gen_random_str();
        ActionHolder action(action_id, this, "handler", m);

        handlers[method].emplace_back(action);
    }

    allow.insert(method);
    return this;
}

}  // namespace ciao
