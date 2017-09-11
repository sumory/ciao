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
    std::string _body;

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
        _body = ctx->body().ToString();
    }

    std::string get_header(const std::string& key) {
        if (key.empty()) return "";
        return _ctx->FindRequestHeader(key.c_str());
    }

    std::string get_query(const std::string& key) {
        if (key.empty()) return "";
        return _ctx->GetQuery(key);
    }

    std::string get_raw_body() { return _body; }

    std::string get_body(const std::string& key) {
        static const std::string s_nullptr = "";
        if (key.empty()) return s_nullptr;

        const char* body = _body.c_str();
        size_t body_len = _body.size();
        size_t key_len = key.size();

        // Find query start point
        for (const char* p = body; p < body + body_len;) {
            size_t index = 0;
            for (; index < key_len; ++index) {
                if (p[index] != key[index]) {
                    break;
                }
            }

            if (index == key_len && p[key_len] == '=') {
                // Found the key
                const char* v = p + key_len + 1;
                const char* end = strchr(const_cast<char*>(v), '&');
                if (!end) {
                    return v;
                } else {
                    return std::string(v, end);
                }
            }

            // Skip to next query
            p += index;
            p = strchr(const_cast<char*>(p), '&');
            if (!p) {
                return s_nullptr;
            }
            p += 1;
        }

        return s_nullptr;
    }

    std::string get_params(const std::string& key) {
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
