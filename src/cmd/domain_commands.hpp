#ifndef YESS_DOMAIN_COMMANDS_HPP
#define YESS_DOMAIN_COMMANDS_HPP

#include "../action_handler.hpp"
#include "command.hpp"
#include <string>

namespace yess::cmd
{
class Domain_command : public Command
{
  public:
    explicit Domain_command(const Action_handler& handler);

  protected:
    const Action_handler& handler_;
};
struct Create_stream_req {
    explicit Create_stream_req(const std::string& type);
    std::string type_;
};

class Create_stream : public Domain_command
{
  public:
    Create_stream(const Action_handler& handler, const Create_stream_req& req);
    Command_result execute() override;
    static std::string usage();

  private:
    const Create_stream_req& request_;
};
struct Delete_stream_req {
    explicit Delete_stream_req(int id);
    int id;
};

class Delete_stream : public Domain_command
{
  public:
    Delete_stream(const Action_handler& handler, const Delete_stream_req& req);
    Command_result execute() override;
    static std::string usage();

  private:
    const Delete_stream_req& request_;
};
struct Get_streams_req {
    std::string type = "";
};

class Get_streams : public Domain_command
{
  public:
    Get_streams(const Action_handler& handler, const Get_streams_req& req);
    Command_result execute() override;
    static std::string usage();

  private:
    const Get_streams_req& request_;
};

struct Push_req {
    explicit Push_req(const int& stream_id,
                      std::string type,
                      std::string payload,
                      const int& version);
    int stream_id_;
    std::string type_;
    std::string payload_;
    int version_;
};

class Push : public Domain_command
{
  public:
    Push(const Action_handler& handler, const Push_req& req);
    Command_result execute() override;
    static std::string usage();

  private:
    const Push_req& request_;
};

struct Create_projection_req {
    Create_projection_req(std::string data, std::string type);
    explicit Create_projection_req(std::string data);
    std::string data;
    std::string type;
};

class Create_projection : public Domain_command
{
  public:
    Create_projection(const Action_handler& handler,
                      const Create_projection_req& req);
    Command_result execute() override;
    static std::string usage();

  private:
    const Create_projection_req& request_;
};

struct Play_req {
    Play_req(int projection_id, int stream_id, json init = json::object());
    Play_req(int projection_id, std::string type, json init = json::object());
    int stream_id_{};
    std::string type;
    int projection_id;
    json init = json::object();
};

class Play : public Domain_command
{
  public:
    Play(const Action_handler& handler, const Play_req& req);
    Command_result execute() override;
    static std::string usage();

  private:
    const Play_req& request_;
};

struct Get_projections_req {
    std::string type = "";
};

class Get_projections : public Domain_command
{
  public:
    Get_projections(const Action_handler& handler,
                    const Get_projections_req& req);
    Command_result execute() override;
    static std::string usage();

  private:
    const Get_projections_req& request_;
};

struct Delete_projection_req {
    int id;
};

class Delete_projection : public Domain_command
{
  public:
    Delete_projection(const Action_handler& handler,
                      const Delete_projection_req& req);
    Command_result execute() override;
    static std::string usage();

  private:
    const Delete_projection_req& request_;
};
} // namespace yess::cmd

#endif // YESS_DOMAIN_COMMANDS_HPP
