#pragma once
#include "ciao/define.h"

namespace ciao {

struct GroupMethodHolder {
    std::string method;
    std::string path;
    std::vector<Middleware> ms;

    GroupMethodHolder(const std::string& m, const std::string& p,
                      std::vector<Middleware> middlewares)
        : method(m), path(p), ms(middlewares) {}
};

// a simple wrapper for related routes
class Group {
 public:
    Group() { _name = "DEFAULT_CIAO_GROUP"; }
    explicit Group(const std::string& name) { _name = name; }
    ~Group() {}

    std::string name() { return _name; }
    std::vector<GroupMethodHolder> get_apis() { return _apis; }

    // 挂载到path指定的group子路由上，不使用模板和宏，显式定义各个方法
    Group& get(const std::string& path, Middleware m) {
        _set_api("get", path, m);
        return *this;
    }
    Group& get(const std::string& path, std::vector<Middleware> ms) {
        _set_api("get", path, ms);
        return *this;
    }
    Group& post(const std::string& path, Middleware m) {
        _set_api("post", path, m);
        return *this;
    }
    Group& post(const std::string& path, std::vector<Middleware> ms) {
        _set_api("post", path, ms);
        return *this;
    }
    Group& del(const std::string& path, Middleware m) {
        _set_api("delete", path, m);
        return *this;
    }
    Group& del(const std::string& path, std::vector<Middleware> ms) {
        _set_api("delete", path, ms);
        return *this;
    }
    Group& put(const std::string& path, Middleware m) {
        _set_api("put", path, m);
        return *this;
    }
    Group& put(const std::string& path, std::vector<Middleware> ms) {
        _set_api("put", path, ms);
        return *this;
    }
    Group& patch(const std::string& path, Middleware m) {
        _set_api("patch", path, m);
        return *this;
    }
    Group& patch(const std::string& path, std::vector<Middleware> ms) {
        _set_api("patch", path, ms);
        return *this;
    }

    // 直接挂载到group根路由
    Group& get(Middleware m) {
        _set_api("get", "", m);
        return *this;
    }
    Group& get(std::vector<Middleware> ms) {
        _set_api("get", "", ms);
        return *this;
    }
    Group& post(Middleware m) {
        _set_api("post", "", m);
        return *this;
    }
    Group& post(std::vector<Middleware> ms) {
        _set_api("post", "", ms);
        return *this;
    }
    Group& del(Middleware m) {
        _set_api("delete", "", m);
        return *this;
    }
    Group& del(std::vector<Middleware> ms) {
        _set_api("delete", "", ms);
        return *this;
    }
    Group& put(Middleware m) {
        _set_api("put", "", m);
        return *this;
    }
    Group& put(std::vector<Middleware> ms) {
        _set_api("put", "", ms);
        return *this;
    }
    Group& patch(Middleware m) {
        _set_api("patch", "", m);
        return *this;
    }
    Group& patch(std::vector<Middleware> ms) {
        _set_api("patch", "", ms);
        return *this;
    }

 private:
    bool _check_method(const std::string& method) {
        if (method.empty()) return false;
        if (supported_http_methods.find(method) != supported_http_methods.end()) {
            return true;
        }
        return false;
    }

    void _set_api(const std::string& method, const std::string& path, Middleware m) {
        std::vector<Middleware> ms{m};
        _set_api(method, path, ms);
    }
    void _set_api(const std::string& method, const std::string& path, std::vector<Middleware> ms) {
        if (!_check_method(method)) {
            std::string err_msg = "method:" + method + " is not supported!";
            CIAO_ERROR(err_msg);
            CIAO_GODIE(err_msg);
        }
        _apis.emplace_back(method, path, ms);
    }

 private:
    std::string _name;
    std::vector<GroupMethodHolder> _apis;
};

}  // namespace ciao
