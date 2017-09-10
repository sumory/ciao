#pragma once
#include "ciao/define.h"
#include "ciao/response.h"
#include "evpp/http/http_server.h"

namespace ciao {

class EvppResponse : public Response {
 private:
    evpp::http::ContextPtr _ctx;
    evpp::http::HTTPSendResponseCallback _cb;

 public:
    EvppResponse(const evpp::http::ContextPtr& ctx, const evpp::http::HTTPSendResponseCallback& cb)
        : _ctx(ctx), _cb(cb) {}

    void response(const std::string& data) {
        _http_status = _http_status == -1 ? 200 : _http_status;
        _ctx->set_response_http_code(_http_status);
        for (auto it = _headers.begin(); it != _headers.end(); it++) {
            _ctx->AddResponseHeader(it->first, it->second);
        }
        _cb(data);
    }
};

}  // namespace ciao
