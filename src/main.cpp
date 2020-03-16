#include "Poco/FileStream.h"
#include "Poco/JSON/Object.h"
#include "Poco/StreamCopier.h"
#include "Server.hpp"
#include "ext/DukContext.hpp"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string>

std::string readAll(Poco::FileInputStream *t_reader)
{
    if (t_reader == NULL) {
        return std::string("");
    }

    t_reader->seekg(0, t_reader->end);
    int length = (int)t_reader->tellg();
    t_reader->seekg(0, t_reader->beg);

    char *buffer = new char[length];
    memset(buffer, 0, length);
    // read data as a block:
    t_reader->read(buffer, length);
    std::string i_data(buffer, length);
    delete[] buffer;
    return i_data;
}

int main(int argc, char *argv[])
{
    /*
duk_context *ctx = duk_create_heap_default();
duk_eval_string(ctx, "1+2");
printf("1+2=%d\n", (int)duk_get_int(ctx, -1));
duk_destroy_heap(ctx);
*/
    /*
Leves::ext::DukContext ctx;
ctx.eval("1+2+3");
void *result = ctx.getResult();
std::cout << "result: " << result << std::endl;
printf("pointer:%p\n", result);
// printf("1+2=%d\n", *(int *)ctx.getResult());
*/

    Poco::FileInputStream fis("bin/projection.js");
    assert(fis.good());
    std::string read;
    Poco::StreamCopier::copyToString(fis, read);
    assert(!read.empty());
    std::cout << "File: " << std::endl;
    std::cout << read << std::endl;
    std::cout << "ENDFILE" << std::endl;

    Leves::ext::DukContext ctx;
    ctx.read(read);
    Poco::JSON::Object::Ptr result = ctx.callProjection("projection", 100);
    // Response ActionHandler::handle(Poco::JSON::Object::Ptr object)
    std::string alma = result->getValue<std::string>("alma");
    std::cout << "Alma: " << alma << std::endl;
    // std::cout << "Pointer: " << (int *)result << std::endl;

    return 0;
}
/*
int main(int argc, char **argv)
{
    Leves::Server server;
    return server.run(argc, argv);
}
*/
