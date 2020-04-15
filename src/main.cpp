#include "daemonizer.hpp"
#include "server.hpp"
#include <iostream>

void sayhello_loop()
{
    while (1) {
        std::cout << "hello\n";
    }
}

int main(int argc, char **argv)
{
    /*
yess::Server server(argc, argv);
return server.run();
*/
    // daemonize(sayhello_loop, "yess");
    yess::Daemonizer::run(
        [](int argc, char **argv) {
            yess::Server server(argc, argv);
            return server.run();
        },
        argc,
        argv);
    return 0;
}

