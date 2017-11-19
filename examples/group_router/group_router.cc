#include "ciao/app.h"

using namespace ciao;

int main(int argc, char* argv[]) {
    App app;
    app.get("/", [](Request& req, Response& res, Next& next) { res.send("hello world!"); });

    Group group1;
    group1
        .get([&](Request& req, Response& res, Next& next) {
            res.send("this is group router: " + group1.name());
        })
        .get("/get",
             [](Request& req, Response& res, Next& next) {
                 std::ostringstream oss;
                 std::string name = req.get_query("name");
                 std::string age = req.get_query("age");
                 std::string desc = req.get_query("desc");  // 中文会被utf-8编码
                 oss << "GET API: " << req.uri << std::endl
                     << " name: " << name << std::endl
                     << " age: " << age << std::endl
                     << " desc: " << desc;
                 res.send(oss.str());
             })
        .post("/post", [](Request& req, Response& res,
                          Next& next) { res.send("group router: post, URI: " + req.path); })
        .del("/delete", [](Request& req, Response& res, Next& next) {
            res.send("group router: delete, URI: " + req.path);
        });

    Group group2("group_router_2");
    group2
        .get([&](Request& req, Response& res, Next& next) {
            res.send("this is root route of group router[" + group2.name() + "]");
        })
        .get("get", [&](Request& req, Response& res, Next& next) {
            res.send("this is `" + req.path + "` of group router[" + group2.name() + "]");
        });

    app.use("/group", group1);
    app.use("/group/2/", group2);

    app.listen(9000).run();
    return 0;
}
