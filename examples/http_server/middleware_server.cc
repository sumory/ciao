#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "ciao/app.h"

using namespace ciao;

int main(int argc, char* argv[]) {
    App app;

    // Hello World!
    app.get("/", [](Request& req, Response& res, Next& next) { res.send("hello world!"); });

    // 以`/test`开头的URI均经过此middleware处理
    app.use("/test", [](Request& req, Response& res, Next& next) {
        if (req.params["injected_p1"].empty() && req.get_query("injected_p1").empty()) {
            req.params["injected_p1"] = "p1";
        }
        if (req.params["injected_p2"].empty()) {
            req.params["injected_p2"] = "p2";
        }
        next("");  // 继续下个路由
    });

    // 测试中间件middleware是否生效
    app.get("/test/case", [](Request& req, Response& res, Next& next) {
        std::ostringstream oss;
        std::string injected_p1 = req.get_params("injected_p1");
        std::string injected_p2 = req.get_params("injected_p2");
        oss << "GET API: " << req.uri << std::endl
            << " injected_p1: " << injected_p1 << std::endl
            << " injected_p2: " << injected_p2 << std::endl;
        res.send(oss.str());
    });

    // 来自uri和post body的参数不会混淆
    app.post("/test/:id/mix", [](Request& req, Response& res, Next& next) {
        std::ostringstream oss;
        std::string injected_p1 = req.get_params("injected_p1");
        if (injected_p1.empty()) {
            injected_p1 = req.get_query("injected_p1");
        }
        std::string injected_p2 = req.get_params("injected_p2");
        std::string id = req.get_params("id");
        oss << "POST API: " << req.uri << std::endl
            << " injected_p1: " << injected_p1 << std::endl
            << " injected_p2: " << injected_p2 << std::endl
            << " id: " << id << std::endl;
        res.send(oss.str());
    });

    app.listen(9000).run();
    return 0;
}
