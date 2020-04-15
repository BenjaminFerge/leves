#include <exception>
#include <iostream>
#include <signal.h>
#include <systemd/sd-daemon.h>

#include "daemonizer.hpp"

namespace yess
{
int Daemonizer::run(int (*cb)(int argc, char **argv), int argc, char **argv)
{
    int result = 1;
    try {
        sd_notify(0, "READY=1");
        std::cerr << std::string("Yess daemon has successfully started up.")
                  << std::endl
                  << std::flush;

        // start program
        result = cb(argc, argv);

        sd_notify(0, "STOPPING=1");
        std::cerr << std::string("Yess daemon has been successfully shut down.")
                  << std::endl
                  << std::flush;
    } catch (std::exception &ex) {
        sd_notifyf(
            0, "STATUS=Failed to start up: %s\n ERRNO=%i", ex.what(), result);
        return result;
    }
    result = 0;
    return result;
}
} // namespace yess
