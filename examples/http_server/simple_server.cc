#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>
#include "ciao/app.h"

using namespace ciao;

int main(int argc, char* argv[]) {
    App app;

    // Hello World!
    app.get("/", [](Request& req, Response& res, Next& next) { res.send("hello world!"); });

    // get请求示例: http://localhost:9000/get?name=mike&age=18&desc=歪国人
    app.get("/get", [](Request& req, Response& res, Next& next) {
        std::ostringstream oss;
        std::string name = req.get_query("name");
        std::string age = req.get_query("age");
        std::string desc = req.get_query("desc");  // 中文会被utf-8编码
        oss << "GET API: " << req.uri << std::endl
            << " name: " << name << std::endl
            << " age: " << age << std::endl
            << " desc: " << desc;
        res.send(oss.str());
    });

    // json响应示例: http://localhost:9000/json?name=mike&age=18&desc=abc
    app.get("/json", [](Request& req, Response& res, Next& next) {
        std::string result;
        std::string name = req.get_query("name");
        std::string age = req.get_query("age");
        std::string desc = req.get_query("desc");
        try {
            rapidjson::Document doc;
            doc.SetObject();
            rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
            doc.AddMember("success", true, allocator);
            doc.AddMember("name", rapidjson::Value(name.c_str(), name.length()), allocator);
            doc.AddMember("age", rapidjson::Value(age.c_str(), age.length()), allocator);
            doc.AddMember("desc", rapidjson::Value(desc.c_str(), desc.length()), allocator);

            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            doc.Accept(writer);
            result = buffer.GetString();
        } catch (const std::exception& e) {
            result = "{\"success\": false}";
        }

        res.json(result);
    });

    // uri params 参数解析示例
    app.get("/params/:id/:name/test", [](Request& req, Response& res, Next& next) {
        std::ostringstream oss;
        std::string id = req.get_params("id");
        std::string name = req.get_params("name");
        std::string not_exist = req.get_params("not_exist");
        oss << "Params test, GET API: " << req.uri << std::endl
            << " id: " << id << std::endl
            << " name: " << name << std::endl
            << " not_exist: " << not_exist;
        res.send(oss.str());
    });

    // post请求示例 http://localhost:9000/post?find=name
    app.post("/post", [](Request& req, Response& res, Next& next) {
        std::string find = req.get_query("find");  //  查找`请求体`里的字段
        if (find.empty()) {
            find = "id";
        }

        std::ostringstream oss;
        std::string raw_body = req.get_raw_body();
        std::string id = req.get_body(find);
        oss << "Post test, API: " << req.uri << std::endl
            << " body[" << find << "]: " << id << std::endl
            << " raw_body: " << raw_body;
        res.send(oss.str());
    });

    app.listen(9000).run();
    return 0;
}
