#pragma once
#include "ciao/define.h"
#include "ciao/router.h"
#include "ciao/utils.h"

namespace ciao {

class App {
 public:
    std::string ciao_build_time;
    std::string app_start_time;
    std::unordered_map<std::string, std::string> settings;
    Router* router;
    Next done;

 public:
    App() {
        extern std::string CIAO_BUILD_TIME;
        ciao_build_time = CIAO_BUILD_TIME;
        app_start_time = get_current_datetime();
        router = new Router();
    }

    explicit App(std::unordered_map<std::string, std::string> s) : settings(s) {
        extern std::string CIAO_BUILD_TIME;
        ciao_build_time = CIAO_BUILD_TIME;
        app_start_time = get_current_datetime();
        router = new Router();
    }

    ~App() { delete router; }

    void run() {
        Request req;
        Response res;
        done = [&, this](const std::string& err) {
            if (!err.empty()) {
                CIAO_ERROR("internal error! please check log.");
                res.status(500).send("internal error! please check log.");
            }
        };
        handle(req, res);
    }

    void run(Next& final_handler) {
        Request req;
        Response res;
        done = final_handler;
        handle(req, res);
    }

    void handle(Request& req, Response& res) {
        bool result = false;
        std::string _err;
        try {
            router->handle(req, res, done);
            result = true;
        } catch (const std::exception& e) {
            _err = "[ERROR]" + std::string(e.what());
            CIAO_ERROR(_err);
            result = false;
        } catch (...) {
            _err = "[ERROR] unknown process request!";
            CIAO_ERROR(_err);
            result = false;
        }

        if (!result) {
            done(_err);
        }
    }

    App* use(const std::string& path, Middleware& m) {
        router->use(path, m);
        return this;
    }

    App* use(const std::string& path, std::vector<Middleware> ms) {
        router->use(path, ms);
        return this;
    }

    App* error_use(const std::string& path, ErrorMiddleware& m) {
        router->error_use(path, m);
        return this;
    }

    App* error_use(const std::string& path, std::vector<ErrorMiddleware> ms) {
        router->error_use(path, ms);
        return this;
    }

    // basic methods start...
    App* get(const std::string& path, Middleware m) {
        router->get(path, m);
        return this;
    }
    App* get(const std::string& path, std::vector<Middleware> ms) {
        router->get(path, ms);
        return this;
    }
    App* post(const std::string& path, Middleware m) {
        router->post(path, m);
        return this;
    }
    App* post(const std::string& path, std::vector<Middleware> ms) {
        router->post(path, ms);
        return this;
    }
    App* del(const std::string& path, Middleware m) {
        router->del(path, m);
        return this;
    }
    App* del(const std::string& path, std::vector<Middleware> ms) {
        router->del(path, ms);
        return this;
    }
    App* put(const std::string& path, Middleware m) {
        router->put(path, m);
        return this;
    }
    App* put(const std::string& path, std::vector<Middleware> ms) {
        router->put(path, ms);
        return this;
    }
    App* patch(const std::string& path, Middleware m) {
        router->patch(path, m);
        return this;
    }
    App* patch(const std::string& path, std::vector<Middleware> ms) {
        router->patch(path, ms);
        return this;
    }
    // basic methods end.
};

}  // namespace ciao
