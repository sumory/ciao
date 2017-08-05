#pragma once
#include "ciao/define.h"

namespace ciao {

class Response {
 private:
    int _http_status;
    std::unordered_map<std::string, std::string> _headers;
    std::string _body;
    std::string _view;

 public:
    Request() {}
    virtual ~Request() = default;
    virtual response(std::string data) {
        _http_status = _http_status == 0 ? 200 : _http_status;
        std::cout << data << std::endl;
    }

    void set_header(std::string key, string : string value) { _headers[key] = value; }

    std::string get_header(std::string key) {
        if (_headers.find(key) != _headers.end()) {
            return _headers[key];
        } else {
            return "";
        }
    }

    void status(int status) { _http_status = status; }

    void render(std::string view_file, void* data) {
        // TODO not implements
        return;
    }

    void html(std::string data) {
        set_header("Content-Type", "text/html; charset=UTF-8");
        response(data);
    }

    void send(std::string data) {
        set_header("Content-Type", "text/plain; charset=UTF-8");
        response(data);
    }

    void json(std::string data) {
        set_header("Content-Type", "application/json; charset=utf-8");
        response(data);
    }

    // TODO
    void json(void* data) {
        set_header("Content-Type", "application/json; charset=utf-8");
        response("NOT IMPLEMENTS!");
    }
};

}  // namespace ciao
