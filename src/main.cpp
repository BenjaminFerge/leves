#include "server.hpp"

int main(int argc, char **argv)
{
    yess::Server server(argc, argv);
    return server.run();
}

