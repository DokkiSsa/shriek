#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <filesystem>

#include "shriek.h"

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
  if ((path = std::getenv("XDG_CONFIG_HOME")) ||
      (path = std::getenv("HOME")))
  {
    return std::string(path) + "/shriek";
  }
  throw std::runtime_error("Neither XDG_CONFIG_HOME nor HOME environment variables are set.");
}

const COM findCommand(const char *command)
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
    std::cerr << error;
  std::cerr << std::endl;
}

Topic *getUserTopic(std::string configPath, const char *topic)
{
  errorList errors;
  if (!isValidTopicName(topic))
  {
    errors.push_back("[Error]: Invalid topic name.\n");
    print_errors(errors);
    return nullptr;
  }
  std::string topicFilePath = configPath + "/" + topic;
  if (!fs::exists(topicFilePath) && !createFile(topicFilePath))
  {
    errors.push_back("[Error]: Could not create file: " + topicFilePath + "\n");
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

int subscribe(std::string configPath, const char *topic, const char *command)
{
  errorList errors;
  Topic *topicObj = nullptr;
  if (!(topicObj = getUserTopic(configPath, topic)))
    return 1;
  if (!isValidCommand(command))
  {
    errors.push_back("[Error]: Invalid command.\n");
    print_errors(errors);
    return 1;
  }
  std::string topicFilePath = configPath + "/" + topic;
  int newId = 1;
  for (const auto &sub : topicObj->subs)
    if (newId > sub.id)
      newId = sub.id + 1;

  topicObj->subs.push_back({newId, command});
  if (!writeTopicFile(topicFilePath, topicObj))
  {
    errors.push_back("[Error]: Could not open file (" + topicFilePath + ") to subscribe .\n");
    print_errors(errors);
    return 1;
  }
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
    errors.push_back("[Error]: Invalid id.\n");
    print_errors(errors);
    return 1;
  }
  std::string topicFilePath = configPath + "/" + topic;
  for (auto &sub : topicObj->subs)
    if (sub.id == id)
    {
      sub.id = -1;
      break;
    }

  if (!writeTopicFile(topicFilePath, topicObj))
  {
    errors.push_back("[Error]: Could not open file (" + topicFilePath + ") to unsubscribe .\n");
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
    errors.push_back("[Error]: Invalid id or command .\n");
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
    errors.push_back("[Error]: Could not open file (" + topicFilePath + ") to update .\n");
    print_errors(errors);
    return 1;
  }
  return 0;
}

int emit(std::string configPath, const char *topic, const char *message)
{
}

int list(const std::string configPath, const char *topic)
{
  if (topic)
  {
    if (!isValidTopicName(topic))
      return 1;
    const std::string subscriptions = getOneFileContentFromPath(configPath, topic);
    if (subscriptions.empty() || subscriptions == COULD_NOT_OPEN_FILE)
      return 1;
    std::cout << "Subscriptions for " << topic << ": \n"
              << subscriptions << std::endl;
  }
  else
  {
    const std::vector<std::string> files = getAllFilesInPath(configPath);
    std::cout << "All topics: \n";
    for (const std::string &file : files)
      std::cout << file << "\n";
    std::cout << std::endl;
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
    // Handle subscribe command
    if (argc < 2)
    {
      std::cerr << "Error: subscribe command requires a topic and a command." << std::endl;
      return 1;
    }
    return subscribe(configPath, argv[0], argv[1]);
    break;
  case COM::UNSUBSCRIBE:
    // Handle unsubscribe command
    if (argc < 2)
    {
      std::cerr << "Error: unsubscribe command requires a topic and an ID." << std::endl;
      return 1;
    }
    return unsubscribe(configPath, argv[0], std::stoi(argv[1]));
    break;
  case COM::UPDATE:
    // Handle update command
    if (argc < 3)
    {
      std::cerr << "Error: update command requires a topic, an ID, and a new command." << std::endl;
      return 1;
    }
    return update(configPath, argv[0], std::stoi(argv[1]), argv[2]);
    break;
  case COM::EMIT:
    // Handle emit command
    if (argc < 2)
    {
      std::cerr << "Error: emit command requires a topic and a message." << std::endl;
      return 1;
    }
    return emit(configPath, argv[0], argv[1]);
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
