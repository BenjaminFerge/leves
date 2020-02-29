#include <iostream>

#include "Server.hpp"

int main(int argc, char **argv)
{
    std::cout << "Starting leves..." << std::endl;
    Leves::Server server;
    return server.run(argc, argv);
}
