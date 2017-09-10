### Ciao <sup>[tʃaʊ]<sup>

a minimal C++ web framework inspired by [lor](https://github.com/sumory/lor) framework which respects [Sinatra](https://github.com/sinatra/sinatra) style.


<details><summary>Example: Hello World!</summary><br>


```c++
#include <ciao/app.h>

int main(int argc, char* argv[]) {
    ciao::App app;
    app.get("/", [](ciao::Request& req, ciao::Response& res, ciao::Next& next){
        res.send("hello world!");
    });
    app.listen(8080).run();
    return 0;
}
```

</details>
