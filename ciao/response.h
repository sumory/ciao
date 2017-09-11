#pragma once
#include "ciao/define.h"

namespace ciao {

class Response {
 protected:
    int _http_status;
    std::unordered_map<std::string, std::string> _headers;
    std::string _body;
    std::string _view;

 public:
    Response() {
        _http_status = -1;
        set_header("Powered By", "ciao");
    }

    virtual ~Response() = default;

    virtual void response(const std::string& data) {
        _http_status = _http_status == -1 ? 200 : _http_status;
    }

    void set_header(std::string key, std::string value) { _headers[key] = value; }

    std::string get_header(const std::string& key) {
        if (_headers.find(key) != _headers.end()) {
            return _headers[key];
        } else {
            return "";
        }
    }

    Response& status(int status) {
        _http_status = status;
        return *this;
    }

    int get_status() { return _http_status; }

    void render(const std::string& view_file, void* data) {
        // TODO not implements
        return;
    }

    void html(const std::string& data) {
        set_header("Content-Type", "text/html; charset=UTF-8");
        response(data);
    }

    void send(const std::string& data) {
        set_header("Content-Type", "text/plain; charset=UTF-8");
        response(data);
    }

    void json(const std::string& data) {
        set_header("Content-Type", "application/json; charset=utf-8");
        response(data);
    }
};

}  // namespace ciao
