#include "ciao/app.h"

using namespace ciao;

int main(int argc, char* argv[]) {
    App app;
    app.get("/get",
            [](Request& req, Response& res, Next& next) { res.send("this is a `get` request"); });

    app.post("/post",
             [](Request& req, Response& res, Next& next) { res.send("this is a `post` request"); });

    app.listen(9000).run();

    return 0;
}
