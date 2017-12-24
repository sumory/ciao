#pragma once
#include "ciao/define.h"
#include "ciao/evpp_driver/evpp_request.h"
#include "ciao/evpp_driver/evpp_response.h"
#include "ciao/group.h"
#include "ciao/router.h"
#include "ciao/utils.h"

namespace ciao {

class App {
 private:
    std::shared_ptr<evpp::http::Server> _evpp_server;

 public:
    std::string ciao_build_time;
    std::string app_start_time;
    std::unordered_map<std::string, std::string> settings;
    Router* router;
    Next done;
    int http_port;
    std::string driver;
    std::string version;

 private:
    void _evpp_run() {
        std::vector<int> ports = {};
        ports.push_back(http_port);
        int thread_num = 3;
        if (settings.find("evpp_server_thread_num") != settings.end()) {
            int tmp = std::stoi(settings["evpp_server_thread_num"]);
            if (tmp > 0) {
                thread_num = tmp;
            }
        }
        _evpp_server = std::make_shared<evpp::http::Server>(thread_num);
        _evpp_server->SetThreadDispatchPolicy(evpp::ThreadDispatchPolicy::kIPAddressHashing);
        _evpp_server->RegisterDefaultHandler([&](evpp::EventLoop* loop,
                                                 const evpp::http::ContextPtr& ctx,
                                                 const evpp::http::HTTPSendResponseCallback& cb) {
            EvppRequest req(ctx);
            EvppResponse res(ctx, cb);
            _run(req, res);
        });
        _evpp_server->Init(ports);
        _evpp_server->Start();
        while (!_evpp_server->IsStopped()) {
            usleep(1);
        }
        CIAO_INFO("evpp_run touches end!");
    }

    void _evpp_stop() {
        if (_evpp_server) {
            CIAO_INFO("evpp server is to stop!");
            _evpp_server->Stop();
            _evpp_server = nullptr;
        }
    }

    void _run(Request& req, Response& res) {
        done = [&](const std::string& err) {
            if (!err.empty()) {
                std::string err_msg = "internal error! please check log. err: " + err;
                CIAO_ERROR(err_msg);
                if (res.get_status() <= 0) {
                    res.status(500);
                }
                res.send(err_msg);
            }
        };

        handle(req, res);
    }

    void _run(Request& req, Response& res, Next& final_handler) {
        done = final_handler;
        handle(req, res);
    }

 public:
    App() {
        extern std::string CIAO_BUILD_TIME;
        ciao_build_time = CIAO_BUILD_TIME;
        app_start_time = get_current_datetime();
        version = std::to_string(CIAO_VERSION_MAJOR) + "." + std::to_string(CIAO_VERSION_MINOR) +
                  "." + std::to_string(CIAO_VERSION_PATCH);
        router = new Router();
        // default config
        driver = "evpp";
    }

    explicit App(std::unordered_map<std::string, std::string> s) : settings(s) {
        extern std::string CIAO_BUILD_TIME;
        ciao_build_time = CIAO_BUILD_TIME;
        app_start_time = get_current_datetime();
        version = std::to_string(CIAO_VERSION_MAJOR) + "." + std::to_string(CIAO_VERSION_MINOR) +
                  "." + std::to_string(CIAO_VERSION_PATCH);
        router = new Router();
        // default config
        driver = "evpp";
    }

    ~App() {
        if (router) delete router;
    }

    void set_driver(std::string driver_name) { driver = driver_name; }

    bool set_settings(const std::string& key, const std::string& value) {
        settings[key] = value;
        return true;
    }

    App& listen(int port) {
        http_port = port;
        return *this;
    }

    void run() {
        CIAO_INFO("server is starting on port " + std::to_string(http_port));
        if (driver == "evpp") {
            _evpp_run();
        } else {
            CIAO_GODIE("no driver was set!");
        }
    }

    bool stop(bool delete_router = false) {
        if (delete_router) {
            delete router;
            router = nullptr;
        }

        if (driver == "evpp") {
            _evpp_stop();
        } else {
            CIAO_GODIE("no driver was set!");
        }

        return true;
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

    App& use(const std::string& path, Middleware m) {
        router->use(path, m);
        return *this;
    }

    App& use(const std::string& path, std::vector<Middleware> ms) {
        router->use(path, ms);
        return *this;
    }

    App& use(const std::string& prefix_path, Group& g) {
        if (prefix_path.empty()) {
            CIAO_ERROR("prefix should not be empty for `group` router");
            return *this;
        }

        for (const GroupMethodHolder& gmh : g.get_apis()) {
            if (gmh.path.empty()) {  // group root route
                router->_init_basic_method(gmh.method, prefix_path, gmh.ms);
                continue;
            }
            std::string final_path = slim_path(prefix_path + "/" + gmh.path);
            router->_init_basic_method(gmh.method, final_path, gmh.ms);
        }
        return *this;
    }

    App& error_use(const std::string& path, ErrorMiddleware& m) {
        router->error_use(path, m);
        return *this;
    }

    App& error_use(const std::string& path, std::vector<ErrorMiddleware> ms) {
        router->error_use(path, ms);
        return *this;
    }

    // basic methods start...，不使用模板和宏，显式定义各个方法，方便后续重构
    App& get(const std::string& path, Middleware m) {
        router->get(path, m);
        return *this;
    }
    App& get(const std::string& path, std::vector<Middleware> ms) {
        router->get(path, ms);
        return *this;
    }
    App& post(const std::string& path, Middleware m) {
        router->post(path, m);
        return *this;
    }
    App& post(const std::string& path, std::vector<Middleware> ms) {
        router->post(path, ms);
        return *this;
    }
    App& del(const std::string& path, Middleware m) {
        router->del(path, m);
        return *this;
    }
    App& del(const std::string& path, std::vector<Middleware> ms) {
        router->del(path, ms);
        return *this;
    }
    App& put(const std::string& path, Middleware m) {
        router->put(path, m);
        return *this;
    }
    App& put(const std::string& path, std::vector<Middleware> ms) {
        router->put(path, ms);
        return *this;
    }
    App& patch(const std::string& path, Middleware m) {
        router->patch(path, m);
        return *this;
    }
    App& patch(const std::string& path, std::vector<Middleware> ms) {
        router->patch(path, ms);
        return *this;
    }

    App& get(Middleware m) {
        router->get("", m);
        return *this;
    }
    App& get(std::vector<Middleware> ms) {
        router->get("", ms);
        return *this;
    }
    App& post(Middleware m) {
        router->post("", m);
        return *this;
    }
    App& post(std::vector<Middleware> ms) {
        router->post("", ms);
        return *this;
    }
    App& del(Middleware m) {
        router->del("", m);
        return *this;
    }
    App& del(std::vector<Middleware> ms) {
        router->del("", ms);
        return *this;
    }
    App& put(Middleware m) {
        router->put("", m);
        return *this;
    }
    App& put(std::vector<Middleware> ms) {
        router->put("", ms);
        return *this;
    }
    App& patch(Middleware m) {
        router->patch("", m);
        return *this;
    }
    App& patch(std::vector<Middleware> ms) {
        router->patch("", ms);
        return *this;
    }
    // basic methods end.

    // stat
    std::vector<std::pair<std::string, std::string>> stat() {
        std::vector<std::pair<std::string, std::string>> result;
        router->trie.walk_trie(result);
        return result;
    }

    std::string text_stat() {
        std::vector<std::pair<std::string, std::string>> result = stat();
        std::string content;
        for (auto& r : result) {
            content += r.first + "\t" + r.second;
            content += "\n";
        }
        return content;
    }
};

}  // namespace ciao
