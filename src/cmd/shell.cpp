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
            return std::make_unique<cmd::Invalid>(Create_stream::usage());
        }
        auto req = new cmd::Create_stream_req(argv[0]);
        return std::make_unique<cmd::Create_stream>(handler_, *req);
    }
    case Shell_cmd::push: {
        if (argv.size() != 4) {
            return std::make_unique<cmd::Invalid>(Push::usage());
        }
        int stream_id;
        int version;
        try {
            stream_id = std::stoi(argv[0]);
            version = std::stoi(argv[3]);
        } catch (std::invalid_argument /* ex */) {
            return std::make_unique<cmd::Invalid>(Push::usage());
        }
        std::string type = argv[1];
        std::string payload = argv[2];
        auto req = new cmd::Push_req(stream_id, type, payload, version);
        return std::make_unique<cmd::Push>(handler_, *req);
    }
    case Shell_cmd::create_projection: {
        // std::unique_ptr<cmd::Create_projection_req> req;
        cmd::Create_projection_req* req;
        if (argv.size() == 1) {
            req = new cmd::Create_projection_req(argv[0]);
        } else if (argv.size() == 2) {
            req = new cmd::Create_projection_req(argv[0], argv[1]);
        } else {
            return std::make_unique<cmd::Invalid>(Create_projection::usage());
        }
        return std::make_unique<cmd::Create_projection>(handler_, *req);
    }
    case Shell_cmd::get_streams: {
        cmd::Get_streams_req* req = new cmd::Get_streams_req;
        if (argv.size() == 1) {
            req->type = argv[0];
        } else if (argv.size() != 0) {
            return std::make_unique<cmd::Invalid>(Get_streams::usage());
        }
        return std::make_unique<cmd::Get_streams>(handler_, *req);
    }
    case Shell_cmd::get_projections: {
        cmd::Get_projections_req* req = new cmd::Get_projections_req;
        if (argv.size() == 1) {
            req->type = argv[0];
        } else if (argv.size() != 0) {
            return std::make_unique<cmd::Invalid>(Get_projections::usage());
        }
        return std::make_unique<cmd::Get_projections>(handler_, *req);
    }
    case Shell_cmd::delete_projection: {
        cmd::Delete_projection_req* req = new cmd::Delete_projection_req;
        if (argv.size() == 1) {
            try {
                req->id = std::stoi(argv[0]);
            } catch (std::invalid_argument /* ex */) {
                return std::make_unique<cmd::Invalid>(
                    Delete_projection::usage());
            }
        } else {
            return std::make_unique<cmd::Invalid>(Delete_projection::usage());
        }
        return std::make_unique<cmd::Delete_projection>(handler_, *req);
    }
        /*
    case Shell_cmd::play: {
        return std::make_unique<cmd::Play>(handler_, *req);
    }*/
    }
}
cmd::Shell::Shell(const Action_handler& handler) : handler_(handler)
{
}
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
        auto d = result.data();
        if (d.has_value()) {
            if (d.type() == typeid(std::vector<db::Stream>)) {
                auto streams = std::any_cast<std::vector<db::Stream>>(d);
                std::cout << streams << std::endl;
            } else if (d.type() == typeid(std::vector<db::Event>)) {
                auto events = std::any_cast<std::vector<db::Event>>(d);
                std::cout << events << std::endl;
            } else if (d.type() == typeid(std::vector<db::Projection>)) {
                auto projections =
                    std::any_cast<std::vector<db::Projection>>(d);
                std::cout << projections << std::endl;
            } else if (d.type() == typeid(db::Stream)) {
                auto stream = std::any_cast<db::Stream>(d);
                std::cout << stream << std::endl;
            } else if (d.type() == typeid(db::Projection)) {
                auto projection = std::any_cast<db::Projection>(d);
                std::cout << projection << std::endl;
            } else if (d.type() == typeid(db::Event)) {
                auto event = std::any_cast<db::Event>(d);
                std::cout << event << std::endl;
            } else if (d.type() == typeid(nullptr)) {
                std::cout << "NULL" << std::endl;
            } else {
                std::cout << result.data().type().name() << std::endl;
                throw std::runtime_error("Unexpected error.");
            }
        }
        if (result.status() == Command_result::Status::exit)
            should_exit = true;
    }
}
std::vector<std::string> tokenize(std::string command)
{
    std::vector<std::string> qargs;
    int len = command.length();
    bool qot = false, sqot = false;
    int arglen;
    for (int i = 0; i < len; i++) {
        int start = i;
        if (command[i] == '\"') {
            qot = true;
        } else if (command[i] == '\'')
            sqot = true;

        if (qot) {
            i++;
            start++;
            while (i < len && command[i] != '\"')
                i++;
            if (i < len)
                qot = false;
            arglen = i - start;
            i++;
        } else if (sqot) {
            i++;
            start++;
            while (i < len && command[i] != '\'')
                i++;
            if (i < len)
                sqot = false;
            arglen = i - start;
            i++;
        } else {
            while (i < len && command[i] != ' ')
                i++;
            arglen = i - start;
        }
        qargs.push_back(command.substr(start, arglen));
    }
    if (qot || sqot)
        throw std::runtime_error("One of the quotes is open");
    return qargs;
}

std::tuple<cmd::Shell::Shell_cmd, std::vector<std::string>>
cmd::Shell::tokens(std::string in)
{
    std::vector<std::string> argv = tokenize(in);
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
    if (first == "create_projection")
        c = cmd::Shell::Shell_cmd::create_projection;
    if (first == "get_streams")
        c = cmd::Shell::Shell_cmd::get_streams;
    if (first == "get_projections")
        c = cmd::Shell::Shell_cmd::get_projections;
    if (first == "delete_projection")
        c = cmd::Shell::Shell_cmd::delete_projection;
    if (first == "quit")
        c = cmd::Shell::Shell_cmd::quit;
    return std::tuple<Shell_cmd, std::vector<std::string>>(c, argv);
}
