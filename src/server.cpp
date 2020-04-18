#include <iostream>
#include <stdexcept>
#include <string>
#include <systemd/sd-daemon.h>

#include "argparse.hpp"
#include "cmd/shell.hpp"
#include "db/repositories/sqlite_stream_repo.hpp"
#include "grpc_service.hpp"
#include "log.hpp"
#include "server.hpp"
#include "version.h"

using namespace yess;
using namespace db;

#include <atomic>
#include <chrono>
#include <iostream>
#include <thread>

#include <signal.h>
#include <unistd.h>

namespace
{
// In the GNUC Library, sig_atomic_t is a typedef for int,
// which is atomic on all systems that are supported by the
// GNUC Library
volatile sig_atomic_t do_shutdown = 0;

// std::atomic is safe, as long as it is lock-free
std::atomic<bool> shutdown_requested = false;
static_assert(std::atomic<bool>::is_always_lock_free);
// or, at runtime: assert( shutdown_requested.is_lock_free() );
} // namespace

void my_signal_handler(int /*signum*/)
{
    // ok, lock-free atomics
    do_shutdown = 1;
    shutdown_requested = true;

    const char str[] = "received signal\n";
    // ok, write is signal-safe
    write(STDERR_FILENO, str, sizeof(str) - 1);

    // UB, unsafe, internal buffers: std::cout << "received signal\n";
    // UB, unsafe, allocates: std::vector<T>(20);
    // unsafe, internal buffers: printf("received signal\n");
}

Server::Server(int argc, char **argv)
{
    log::init_logger();
    argparser_ = argparse::ArgumentParser("yess");
    parse_args(argc, argv);
}

Server::Server()
{
    log::init_logger();
    argparser_ = argparse::ArgumentParser("yess");
}

void Server::parse_args(int argc, char **argv)
{
    argparser_.add_argument("-v", "--version")
        .help("print version number")
        .default_value(false)
        .implicit_value(true);

    argparser_.add_argument("-c", "--conn-str")
        .default_value(conn_str_)
        .help("connection string");

    argparser_.add_argument("-p", "--log-path")
        .help("file log path")
        .default_value(std::string(""));

    argparser_.add_argument("--file-only")
        .help("log into file only")
        .default_value(false)
        .implicit_value(true);

    argparser_.add_argument("-d", "--daemon")
        .help("run in background")
        .default_value(false)
        .implicit_value(true);

    argparser_.add_argument("-i", "--interactive")
        .help("interactive shell")
        .default_value(false)
        .implicit_value(true);

    try {
        argparser_.parse_args(argc, argv);
    } catch (const std::runtime_error &err) {
        std::cout << err.what() << std::endl;
        std::cout << argparser_;
        exit(0);
    }

    if (argparser_["--version"] == true) {
        display_version();
        exit(0);
    }

    if (argparser_["--interactive"] == true) {
        log::disable_cout();
        shell();
        exit(0);
    }

    if (argparser_["--daemon"] == true) {
        daemon_ = true;
    }

    auto log_path = argparser_.get<std::string>("--log-path");
    bool file_only = argparser_.get<bool>("--file-only");
    if (!log_path.empty()) {
        log_path_ = log_path;
        log::info(
            "Logging path is set to: {} (file only: {})", log_path_, file_only);
        log::rotating_logger(log_path_, 5, 3, file_only);
    } else if (file_only) {
        std::cerr << "Please specify --log-path option when --file-only is set"
                  << std::endl;
        exit(1);
    }

    auto conn_str = argparser_.get<std::string>("--conn-str");
    if (!conn_str.empty()) {
        conn_str_ = conn_str;
        log::info("Connection string is set to: {}", conn_str_);
    }
}

Server::~Server() {}

void Server::initDB()
{
    auto streamRepository = Sqlite_stream_repo("SQLite", conn_str_);
    streamRepository.initDB();
}

void Server::display_version()
{
    std::cout << YESS_NAME << " v" << YESS_VER << std::endl;
}

std::string Server::conn_str() { return conn_str_; }

#include <thread>

int Server::run()
{
    // setup signal handler
    {
        struct sigaction action;
        action.sa_handler = my_signal_handler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;
        sigaction(SIGTERM, &action, NULL);
    }

    std::string server_address("0.0.0.0:" + std::to_string(port_));
    Grpc_service service(conn_str_);

    grpc::EnableDefaultHealthCheckService(true);
    grpc::reflection::InitProtoReflectionServerBuilderPlugin();
    grpc::ServerBuilder builder;
    // Listen on the given address without any authentication mechanism.
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // Register "service" as the instance through which we'll communicate with
    // clients. In this case it corresponds to an *synchronous* service.
    builder.RegisterService(&service);
    // Finally assemble the server.
    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    log::info("yess listening on {}...", server_address);

    if (daemon_) {
        // Watchdog thread (for systemd notification)
        std::thread w_th([&]() {
            // TODO: remove 5s delay for SIGTERM handling
            while (!do_shutdown && !shutdown_requested.load()) {
                // log::info("Notify systemd");
                sd_notify(0, "WATCHDOG=1");
                // WatchdogSec=6 (systemd) -> notify every 5 sec
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
            server->Shutdown();
            log::info("Server has been shut down.");
        });
        w_th.join();
    }
    std::thread grpc_th([&]() {
        // Other thread is responsible for shutting down the server
        server->Wait();
    });
    grpc_th.join();

    return 0;
}

void Server::shell()
{
    auto handler = Action_handler(conn_str_);
    yess::cmd::Shell sh(std::move(handler));
    sh.run();
}
