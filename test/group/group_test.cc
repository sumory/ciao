#include "ciao/app.h"
#include "ciao/utils.h"
#include "test/test_common.h"

TEST_F(GroupTest, DefineGroupTest) {
    ciao::Group group;
    ASSERT_STREQ("DEFAULT_CIAO_GROUP", group.name().c_str());
}

TEST_F(GroupTest, DefineGroupMethodTest) {
    ciao::Group group;
    group.get("/abc", [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {});
    group.post("/abc", [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {});
    group.del("/abc", [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {});
    group.put("/abc", [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {});
    group.patch("/abc", [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {});
    std::vector<ciao::Middleware> ms = {
        [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {},
        [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {},
        [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {}};
    group.get("/batch", ms);
    ASSERT_EQ(6lu, group.get_apis().size());
}

TEST_F(GroupTest, AddGroupToAppTest) {
    ciao::App app;
    app.get("/", [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {});

    ciao::Group group;
    group.get([](ciao::Request& req, ciao::Response& res, ciao::Next& next) {});
    group.get("/abc", [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {});
    group.post("/abc", [](ciao::Request& req, ciao::Response& res, ciao::Next& next) {});

    app.use("/group", group);
}
