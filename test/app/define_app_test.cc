#include "ciao/app.h"
#include "ciao/utils.h"
#include "test/test_common.h"

TEST_F(AppTest, DefineAppTest) {
    ciao::App app;

    ASSERT_FALSE(app.app_start_time.empty());
    ASSERT_NE(nullptr, app.router);
}

TEST_F(AppTest, DefineAppWithParamsTest) {
    std::unordered_map<std::string, std::string> settings;
    ciao::App app(settings);

    ASSERT_FALSE(app.app_start_time.empty());
    ASSERT_NE(nullptr, app.router);
}

TEST_F(AppTest, AppRootRouteTest) {
    ciao::App app;
    ciao::Middleware m = [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {};
    app.get(m);
    app.post(m);
    app.put({m,m});

    ASSERT_EQ(1, app.router->trie.add_node("")->handlers["get"].size());
    ASSERT_EQ(1, app.router->trie.add_node("")->handlers["post"].size());
    ASSERT_EQ(2, app.router->trie.add_node("")->handlers["put"].size());
}

TEST_F(AppTest, DefineHttpMethodTest) {
    ciao::App app;
    app.get("/get", [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {});

    ciao::Middleware m = [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {};
    app.post("/post", m);

    ciao::Middleware m2 = [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {};
    app.del("/delete", m2);
    app.put("/put", m2);
    app.patch("/patch", m2);

    ASSERT_FALSE(app.app_start_time.empty());
    ASSERT_NE(nullptr, app.router);
}

TEST_F(AppTest, RedifineForbiddenTest) {
    ciao::App app;

    ciao::Middleware m1 = [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {};
    app.get("/get", m1);

    ciao::Middleware m2 = [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {};
    // not allow redefine the same route
    EXPECT_EXIT(app.get("/get", m2), testing::ExitedWithCode(1), "");
}

TEST_F(AppTest, HandlerSizeTest) {
    ciao::App app;

    ciao::Middleware m1 = [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {};
    app.get("", m1);
    ASSERT_EQ(1, app.router->trie.add_node("")->handlers["get"].size());

    app.get("/abc", m1);
    ASSERT_EQ(1, app.router->trie.add_node("/abc")->handlers["get"].size());

    std::vector<ciao::Middleware> ms = {
        [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {},
        [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {},
        [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {}};

    app.post("/", ms);
    ASSERT_EQ(ms.size(), app.router->trie.add_node("/")->handlers["post"].size());
}
