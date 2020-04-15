#pragma once

namespace yess
{
class Daemonizer
{
  public:
    // static int run(int (*cb)(const int &argc, const char **arbv));
    static int run(int (*cb)(int argc, char **argv), int argc, char **argv);
    /*
static int run(int (*cb)(const int &argc, const char **arbv),
             const int &argc,
             const char **argv);
             */
};
} // namespace yess
