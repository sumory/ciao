#include "ciao/app.h"
#include "test/test_common.h"

TEST_F(AppTest, ApiStatTest) {
    ciao::App app;
    auto mw = [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {};
    app.get("/get", mw);
    app.del("/delete", mw);
    app.put("/put", mw);
    app.patch("/patch", mw);

    app.get("/group/view", mw);
    app.get("/group/:id", mw);
    app.post("/group/:id", mw);
    app.del("/group/:id", mw);
    app.get("/group/:id/:number", mw);

    app.get("/user", mw);
    app.del("/user/delete", mw);
    app.put("/user/update", mw);
    app.patch("/user/modify", mw);

    app.get("/article/view", mw);
    app.post("/article/modify", mw);

    auto stat = app.stat();
    ASSERT_EQ(15lu, stat.size());
    // std::cout << app.text_stat() << std::endl;
}
