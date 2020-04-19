#include "shell.hpp"
// TODO: fix path
#include "../../build/debug/include/version.h"
#include "domain_commands.hpp"
#include "shell_commands.hpp"
#include <sstream>

using namespace yess;

std::unique_ptr<cmd::Command> cmd::Shell::interpret(std::string in)
{
    std::tuple<cmd::Shell::Shell_cmd, std::vector<std::string>> cmd_argv =
        tokens(in);
    auto c = std::get<0>(cmd_argv);
    auto argv = std::get<1>(cmd_argv);
    switch (c) {
    case Shell_cmd::none: {
        return std::make_unique<cmd::None>();
    }
    case Shell_cmd::unknown: {
        return std::make_unique<cmd::Unknown>();
    }
    case Shell_cmd::quit: {
        return std::make_unique<cmd::Quit>();
    }
    case Shell_cmd::help: {
        return std::make_unique<cmd::Help>();
    }
    case Shell_cmd::create_stream: {
        if (argv.size() != 1) {
            // TODO: provide usage hints
            return std::make_unique<cmd::Unknown>();
        }
        auto req = new cmd::Create_stream_req(argv[0]);
        return std::make_unique<cmd::Create_stream>(handler_, *req);
    }
    case Shell_cmd::push: {
        if (argv.size() != 4) {
            // TODO: provide usage hints
            return std::make_unique<cmd::Unknown>();
        }
        int stream_id;
        int version;
        try {
            stream_id = std::stoi(argv[0]);
            version = std::stoi(argv[3]);
        } catch (std::invalid_argument /* ex */) {
            return std::make_unique<cmd::Unknown>();
        }
        std::string type = argv[1];
        std::string payload = argv[2];
        auto req = new cmd::Push_req(stream_id, type, payload, version);
        return std::make_unique<cmd::Push>(handler_, *req);
    }
    }
}
cmd::Shell::Shell(const Action_handler &handler) : handler_(handler) {}
void cmd::Shell::execute(const cmd::Command &cmd) {}
void cmd::Shell::run()
{
    std::cout << "yess interactive shell (";
    std::cout << YESS_VER << ")" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter 'help' for usage hints." << std::endl;
    bool should_exit = false;
    while (!should_exit) {
        std::cout << prompt_;
        std::string line;
        std::getline(std::cin, line);
        if (line.empty())
            continue;
        std::unique_ptr<cmd::Command> cmd = interpret(line);
        cmd::Command_result result = cmd->execute();
        std::cout << result.message() << std::endl;
        if (result.status() == Command_result::Status::exit)
            should_exit = true;
    }
}
std::vector<std::string> to_words(std::string in)
{
    std::vector<std::string> result;
    std::istringstream ss(in);
    do {
        std::string word;
        ss >> word;
        if (!word.empty())
            result.push_back(word);
    } while (ss);
    return result;
}

std::tuple<cmd::Shell::Shell_cmd, std::vector<std::string>>
cmd::Shell::tokens(std::string in)
{
    std::vector<std::string> argv = to_words(in);
    if (argv.size() == 0) {
        return std::tuple<Shell_cmd, std::vector<std::string>>(
            cmd::Shell::Shell_cmd::none, {});
    }
    std::string first = argv[0];
    argv.erase(argv.begin());

    auto c = cmd::Shell::Shell_cmd::unknown;
    if (first == "help")
        c = cmd::Shell::Shell_cmd::help;
    if (first == "push")
        c = cmd::Shell::Shell_cmd::push;
    if (first == "create_stream")
        c = cmd::Shell::Shell_cmd::create_stream;
    if (first == "quit")
        c = cmd::Shell::Shell_cmd::quit;
    return std::tuple<Shell_cmd, std::vector<std::string>>(c, argv);
}
