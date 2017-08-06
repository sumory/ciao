#pragma once
#include <inttypes.h>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "ciao/config.h"

#define CIAO_DEBUG(str) std::cout << "[DEBUG]" << str << std::endl;
#define CIAO_INFO(str) std::cout << "[INFO]" << str << std::endl;
#define CIAO_WARN(str) std::cout << "[WARN]" << str << std::endl;
#define CIAO_ERROR(str) std::cout << "[ERROR]" << str << std::endl;

#define CIAO_GODIE(str)                             \
    do {                                            \
        std::cerr << "[FATAL]" << str << std::endl; \
        exit(1);                                    \
    } while (0);

namespace ciao {

#ifdef INJECT_BUILD_DATE
extern std::string CIAO_BUILD_TIME;
#else
extern std::string CIAO_BUILD_TIME;
#endif

extern std::unordered_map<std::string, bool> supported_http_methods;

class Request;
class Response;

// err不为空则出错
typedef std::function<void(const std::string& err)> Next;
typedef std::function<void(Request& req, Response& res, Next& next)> Middleware;
typedef std::function<void(const std::string& err, Request& req, Response& res, Next& next)>
    ErrorMiddleware;

}  // namespace ciao
