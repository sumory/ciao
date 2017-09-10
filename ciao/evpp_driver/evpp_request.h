#pragma once
#include <sstream>
#include "ciao/define.h"
#include "ciao/request.h"
#include "evpp/http/http_server.h"
#include "evpp/libevent.h"

namespace ciao {

class EvppRequest : public Request {
 private:
    evpp::http::ContextPtr _ctx;

 public:
    explicit EvppRequest(const evpp::http::ContextPtr& ctx) : Request(), _ctx(ctx) {
        path = ctx->uri();
        host = std::string(ctx->FindRequestHeader("Host"));
        switch (evhttp_request_get_command(ctx->req())) {
            case EVHTTP_REQ_GET:
                method = "get";
                break;
            case EVHTTP_REQ_POST:
                method = "post";
                break;
            case EVHTTP_REQ_HEAD:
                method = "head";
                break;
            case EVHTTP_REQ_PUT:
                method = "put";
                break;
            case EVHTTP_REQ_DELETE:
                method = "delete";
                break;
            case EVHTTP_REQ_OPTIONS:
                method = "options";
                break;
            case EVHTTP_REQ_TRACE:
                method = "trace";
                break;
            case EVHTTP_REQ_CONNECT:
                method = "connect";
                break;
            case EVHTTP_REQ_PATCH:
                method = "patch";
                break;
            default:
                method = "unknown";
                break;
        }
        url = ctx->original_uri();
        origin_uri = ctx->original_uri();
        uri = ctx->uri();
        body = ctx->body().ToString();
    }

    std::string get_header(std::string key) {
        if (key.empty()) return "";
        return _ctx->FindRequestHeader(key.c_str());
    }

    std::string get_query(std::string key) {
        if (key.empty()) return "";
        return _ctx->GetQuery(key);
    }

    std::string get_body(std::string key) { return body; }

    std::string get_params(std::string key) {
        if (key.empty() || params.find(key) == params.end()) {
            return "";
        } else {
            return params[key];
        }
    }

    std::string to_string() {
        std::ostringstream oss;
        oss << "[EvppRequest path:" << path << ", method:" << method << ", uri:" << uri
            << ", url:" << url << ", body:" << body << "]";
        return oss.str();
    }
};

}  // namespace ciao
