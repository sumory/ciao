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
    std::string host;
    std::unordered_map<std::string, std::string> query;
    std::unordered_map<std::string, std::string> params;
    std::string url;
    std::string origin_uri;
    std::string uri;
    std::string body;

    Request() { _found = false; }
    virtual ~Request() = default;
    virtual std::string get_header(std::string key) = 0;
    virtual std::string get_query(std::string key) = 0;
    virtual std::string get_body(std::string key) = 0;
    virtual std::string get_params(std::string key) = 0;
    virtual std::string to_string() = 0;

    bool is_found() { return _found; }
    void set_found(bool found) { _found = found; }
};

}  // namespace ciao
