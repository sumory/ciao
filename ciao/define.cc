#include "ciao/define.h"

namespace ciao {

#ifdef INJECT_BUILD_DATE
std::string CIAO_BUILD_TIME = "1";
#else
std::string CIAO_BUILD_TIME = "0";
#endif

std::unordered_map<std::string, bool> supported_http_methods({
    {"get", true},
    {"put", true},
    {"post", true},
    {"patch", true},
    {"delete", true},
    {"head", true},
    {"options", true},
    {"all", true},
});

}  // namespace ciao
