### Ciao <sup>[tʃaʊ]<sup>

 [![GitHub release](https://img.shields.io/github/release/sumory/ciao.svg)](https://github.com/sumory/ciao/releases/latest) [![license](https://img.shields.io/github/license/sumory/ciao.svg)](https://github.com/sumory/ciao/blob/master/LICENSE)

a minimal C++ web framework inspired by [lor](https://github.com/sumory/lor) framework which respects [Sinatra](https://github.com/sinatra/sinatra) style.


```c++
#include <ciao/app.h>

using namespace ciao;

int main(int argc, char* argv[]) {
    App app;
    app.get("/", [](Request& req, Response& res, Next& next){
        res.send("hello world!");
    });
    app.listen(8080).run();
    return 0;
}
```

### Features

- [Sinatra](http://www.sinatrarb.com/) routing style
- Middleware support
- Group router support
- HTTP API Server oriented design
- Easy to integrate with other HTTP server/library

### License

[MIT](./LICENSE) License