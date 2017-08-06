#pragma once
#include "ciao/define.h"

namespace ciao {

class Request {
 private:
    bool _found;

 public:
    std::unordered_map<std::string, std::string> headers;
    std::string path;
    std::string method;
    std::unordered_map<std::string, std::string> query;
    std::unordered_map<std::string, std::string> params;
    std::string url;
    std::string origin_uri;
    std::string uri;

    Request() {}
    virtual ~Request() = default;
    virtual bool build(void* data) { return true; };

    bool is_found() { return _found; }

    void set_found(bool found) { _found = found; }
};

}  // namespace ciao
