#include "EchoServer.cpp"
#include <iostream>

int main(int argc, char **argv)
{
    std::cout << "Starting leves..." << std::endl;
    EchoServer app;
    return app.run(argc, argv);
}
