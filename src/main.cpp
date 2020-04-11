#include "server.hpp"

int main(int argc, char **argv)
{
    yess::Server server;
    return server.run(argc, argv);
}
