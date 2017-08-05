#pragma once
#include "ciao/define.h"
#include "ciao/utils.h"

namespace ciao {

struct App {
    std::string ciao_build_time;
    std::string app_start_time;
    std::unordered_map<std::string, std::string> settings;

    App() {
        extern std::string CIAO_BUILD_TIME;
        ciao_build_time = CIAO_BUILD_TIME;
        app_start_time = get_current_datetime();
    }

    explicit App(std::unordered_map<std::string, std::string> s) : settings(s) {
        extern std::string CIAO_BUILD_TIME;
        ciao_build_time = CIAO_BUILD_TIME;
        app_start_time = get_current_datetime();
    }
};

}  // namespace ciao
