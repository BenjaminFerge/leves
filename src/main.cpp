#include "daemonizer.hpp"
#include "server.hpp"

int main(int argc, char **argv)
{
    yess::Daemonizer::run(
        [](int argc, char **argv) {
            yess::Server server(argc, argv);
            return server.run();
        },
        argc,
        argv);
    return 0;
}

