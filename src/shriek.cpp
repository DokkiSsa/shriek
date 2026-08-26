#include <iostream>
#include <filesystem>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "shriek.hpp"

namespace fs = std::filesystem;

#ifndef VERSION
#define VERSION "1.0.0"
#endif

enum class COM
{
  HELP = -1,
  SUBSCRIBE = 1,
  UNSUBSCRIBE = 2,
  UPDATE = 3,
  EMIT = 4,
  LIST = 5,
  VALIDATE = 6,
};

std::string getConfigPath()
{
  char *path;
  std::string fullpath = "";
  if ((path = std::getenv("XDG_CONFIG_HOME")))
  {
    fullpath = std::string(path) + "/shriek";
  }
  if ((path = std::getenv("HOME")))
  {
    fullpath = std::string(path) + "/.config/shriek";
  }
  if (fullpath.empty())
  {
    throw std::runtime_error("Neither XDG_CONFIG_HOME nor HOME environment variables are set.");
  }
  if (!fs::is_directory(fullpath))
  {
    fs::create_directory(fullpath);
  }
  return fullpath;
}

COM findCommand(const char *command)
{
  if (strcmp(command, "subscribe") == 0 ||
      strcmp(command, "when") == 0)
  {
    return COM::SUBSCRIBE;
  }
  else if (strcmp(command, "unsubscribe") == 0 ||
           strcmp(command, "hush") == 0)
  {
    return COM::UNSUBSCRIBE;
  }
  else if (strcmp(command, "update") == 0 ||
           strcmp(command, "retune") == 0)
  {
    return COM::UPDATE;
  }
  else if (strcmp(command, "emit") == 0 ||
           strcmp(command, "at") == 0 ||
           strcmp(command, "about") == 0)
  {
    return COM::EMIT;
  }
  else if (strcmp(command, "list") == 0 ||
           strcmp(command, "topics") == 0 ||
           strcmp(command, "victims") == 0)
  {
    return COM::LIST;
  }
  else if (strcmp(command, "validate") == 0 ||
           strcmp(command, "clear-throat") == 0 ||
           strcmp(command, "soundcheck") == 0 ||
           strcmp(command, "mictesting123") == 0)
  {
    return COM::VALIDATE;
  }
  else
  {
    return COM::HELP;
  }
}

void print_help()
{
  std::cout << "Usage: " << VERSION << "\n"
            << "  shriek subscribe TOPIC COMMAND\n"
            << "  shriek unsubscribe TOPIC ID\n"
            << "  shriek update TOPIC ID COMMAND\n"
            << "  shriek emit TOPIC [MESSAGE]\n"
            << "  shriek list [TOPIC]\n"
            << "  shriek validate\n"
            << "  shriek --help\n";
  std::cout << "\n";
  std::cout << "Silly Usage: " << VERSION << "\n"
            << "  shriek when TOPIC COMMAND\n"
            << "  shriek hush TOPIC ID\n"
            << "  shriek tune TOPIC ID COMMAND\n"
            << "  shriek at/about TOPIC [MESSAGE]\n"
            << "  shriek topics/victims [TOPIC]\n"
            << "  shriek clear-throat/soundcheck/mictesting123\n"
            << "  shriek --help\n";
}

void print_errors(const errorList &errors)
{
  for (const std::string &error : errors)
    std::cerr << error << "\n";
  std::cerr << std::endl;
}

Topic *getUserTopic(std::string configPath, const char *topic)
{
  errorList errors;
  if (!isValidTopicName(topic))
  {
    errors.push_back("[Error]: Invalid topic name.");
    print_errors(errors);
    return nullptr;
  }
  const std::string topicFilePath = configPath + "/" + topic;
  if (!fs::exists(topicFilePath) && !createFile(topicFilePath))
  {
    errors.push_back("[Error]: Could not create file: " + topicFilePath);
    print_errors(errors);
    return nullptr;
  }
  Topic *topicObj = parseTopicFromFile(configPath, topic, errors);
  if (!topic)
  {
    print_errors(errors);
    return nullptr;
  }
  return topicObj;
}

int spawnCommand(const std::string &command, const std::vector<std::string> &envVars)
{
  pid_t pid = fork();
  if (pid < 0)
    return 1;
  if (pid > 0)
  {
    waitpid(pid, nullptr, 0);
    return 0;
  }
  setsid();
  pid_t pid2 = fork();
  if (pid2 < 0)
    _exit(1);
  if (pid2 > 0)
    _exit(0);

  // Second child (fully detached daemon process)
  // Change working directory if needed, close standard file descriptors, etc.
  if (chdir("/") != 0)
    _exit(1);
  std::vector<char *> envp;
  for (const auto &var : envVars)
    envp.push_back(const_cast<char *>(var.c_str()));
  envp.push_back(nullptr);

  char *const argv[] = {
      const_cast<char *>("sh"),
      const_cast<char *>("-c"),
      const_cast<char *>("export SHRIEK_DEPTH;"),
      const_cast<char *>(command.c_str()),
      nullptr};
  execve("/bin/sh", argv, envp.data());

  _exit(1);
}

int subscribe(std::string configPath, const char *topic, const char *command)
{
  errorList errors;
  Topic *topicObj = nullptr;
  if (!(topicObj = getUserTopic(configPath, topic)))
    return 1;
  if (!isValidCommand(command))
  {
    errors.push_back("[Error]: Invalid command.");
    print_errors(errors);
    return 1;
  }
  const std::string topicFilePath = configPath + "/" + topic;
  int newId = 1;
  bool existing = false;
  for (const auto &sub : topicObj->subs)
  {
    if (newId > sub.id)
      newId = sub.id + 1;
    if (sub.command == command)
    {
      existing = true;
      newId = sub.id;
      break;
    }
  }
  if (!existing)
  {
    topicObj->subs.push_back({newId, command});
    if (!writeTopicFile(topicFilePath, topicObj))
    {
      errors.push_back("[Error]: Could not open file (" + topicFilePath + ") to subscribe.");
      print_errors(errors);
      return 1;
    }
  }
  std::cout << newId << std::endl;
  return 0;
}

int unsubscribe(std::string configPath, const char *topic, int id)
{
  errorList errors;
  Topic *topicObj = nullptr;
  if (!(topicObj = getUserTopic(configPath, topic)))
    return 1;
  if (!isValidId(id))
  {
    errors.push_back("[Error]: Invalid id.");
    print_errors(errors);
    return 1;
  }
  const std::string topicFilePath = configPath + "/" + topic;
  for (auto &sub : topicObj->subs)
    if (sub.id == id)
    {
      sub.id = -1;
      break;
    }
  bool deleteCheck = true;
  for (const auto &sub : topicObj->subs)
    if (sub.id != -1)
    {
      deleteCheck = false;
      break;
    }

  if (deleteCheck)
  {
    deleteFile(topicFilePath);
  }
  else if (!writeTopicFile(topicFilePath, topicObj))
  {
    errors.push_back("[Error]: Could not open file (" + topicFilePath + ") to unsubscribe.");
    print_errors(errors);
    return 1;
  }
  return 0;
}

int update(std::string configPath, const char *topic, int id, const char *command)
{
  errorList errors;
  Topic *topicObj = nullptr;
  if (!(topicObj = getUserTopic(configPath, topic)))
    return 1;
  if (!isValidCommand(command) || !isValidId(id))
  {
    errors.push_back("[Error]: Invalid id or command.");
    print_errors(errors);
    return 1;
  }
  std::string topicFilePath = configPath + "/" + topic;
  for (auto &sub : topicObj->subs)
    if (sub.id == id)
    {
      sub.command = command;
      break;
    }
  if (!writeTopicFile(topicFilePath, topicObj))
  {
    errors.push_back("[Error]: Could not open file (" + topicFilePath + ") to update.");
    print_errors(errors);
    return 1;
  }
  return 0;
}

int emit(std::string configPath, const char *topic, const char *message)
{
  errorList errors;
  Topic *topicObj = nullptr;
  if (!(topicObj = getUserTopic(configPath, topic)))
    return 1;
  const std::string topicFilePath = configPath + "/" + topic;
  const int depth = std::stoi(std::getenv("SHRIEK_DEPTH")) + 1;
  for (const auto &sub : topicObj->subs)
  {
    std::vector<std::string> envs = {
        std::string("SHRIEK_TOPIC=") + topic,
        std::string("SHRIEK_MESSAGE=") + message,
        std::string("SHRIEK_DEPTH=") + std::to_string(depth),
        std::string("SHRIEK_SUB_ID=") + std::to_string(sub.id),
    };
    if (spawnCommand(sub.command, envs))
      errors.push_back("[Error]: Could not execute command (id:" + std::to_string(sub.id) + ").");
  }
  if (errors.size())
  {
    print_errors(errors);
    return 1;
  }
  return 0;
}

int list(const std::string configPath, const char *topic)
{
  if (topic)
  {
    if (!isValidTopicName(topic))
      return 1;
    const std::string subscriptions = readFile(configPath + "/" + topic);
    if (subscriptions.empty() || subscriptions == COULD_NOT_OPEN_FILE)
      return 1;
    std::cout << subscriptions << std::endl;
  }
  else
  {
    const std::vector<std::string> files = getAllFilesInPath(configPath);
    if (files.size())
    {
      for (const std::string &file : files)
        std::cout << file << "\n";
      std::cout << std::endl;
    }
  }
  return 0;
}

int validate(std::string configPath)
{
  const std::vector<std::string> files = getAllFilesInPath(configPath);
  errorList errors;
  bool errored = false;
  for (const std::string &file : files)
  {
    errors.clear();
    parseTopicFromFile(configPath, file, errors);
    if (errors.size())
    {
      errored = true;
      print_errors(errors);
    }
  }
  return errored;
}

int main(int argc, char *argv[])
{
  argc--;
  argv++; // skip the program name
  if (argc < 1)
  {
    print_help();
    return 1;
  }
  const COM command = findCommand(argv[0]);
  argc--;
  argv++; // skip the command
  std::string configPath = getConfigPath();
  switch (command)
  {
  case COM::SUBSCRIBE:
    if (argc < 2)
    {
      std::cerr << "Error: subscribe command requires a topic and a command." << std::endl;
      return 1;
    }
    return subscribe(configPath, argv[0], argv[1]);
    break;
  case COM::UNSUBSCRIBE:
    if (argc < 2)
    {
      std::cerr << "Error: unsubscribe command requires a topic and an ID." << std::endl;
      return 1;
    }
    return unsubscribe(configPath, argv[0], std::stoi(argv[1]));
    break;
  case COM::UPDATE:
    if (argc < 3)
    {
      std::cerr << "Error: update command requires a topic, an ID, and a new command." << std::endl;
      return 1;
    }
    return update(configPath, argv[0], std::stoi(argv[1]), argv[2]);
    break;
  case COM::EMIT:
    if (argc < 1)
    {
      std::cerr << "Error: emit command requires a topic and an optional message." << std::endl;
      return 1;
    }
    return emit(configPath, argv[0], argc == 2 ? argv[1] : "");
    break;
  case COM::LIST:
    return list(configPath, argc ? argv[0] : nullptr);
    break;
  case COM::VALIDATE:
    return validate(configPath);
    break;

  default:
    print_help();
    break;
  }
  return 0;
}
