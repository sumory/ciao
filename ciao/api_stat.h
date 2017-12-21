#pragma once
#include "ciao/app.h"

namespace ciao {

class ApiStat {
 public:
    ApiStat(App* app) : _app(app) {}
    ~ApiStat() {}

    std::string text_stat() {
        std::vector<std::pair<std::string, std::string>> result = stat();
        std::string content;
        for (auto& r : result) {
            content += r.first + "\t" + r.second;
            content += "\n";
        }
        return content;
    }

    std::vector<std::pair<std::string, std::string>> stat() {
        std::vector<std::pair<std::string, std::string>> result;
        _app->router->trie.walk_trie(result);
        return result;
    }

 private:
    App* _app;
};

}  // namespace ciao