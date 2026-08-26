#include <iostream>
#include <cstring>
#include <cstdlib>
#include <sstream>

#include "shriek.h"

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

int subscribe(std::string configPath, const char *topic, const char *command)
{
}

int unsubscribe(std::string configPath, const char *topic, int id)
{
}

int update(std::string configPath, const char *topic, int id, const char *command)
{
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
    if (subscriptions.empty())
      return 1;
    std::cout << "Subscriptions for " << topic << ": \n"
              << subscriptions << std::endl;
  }
  else
  {
    const std::vector<std::string> files = getAllFilesInPath(configPath);
    std::cout << "All topics: \n";
    for (const std::string &file : files)
    {
      const int idx = file.find_last_of('/');
      std::cout << file.substr(idx + 1) << "\n";
    }
    std::cout << std::endl;
  }
  return 0;
}

int validate(std::string configPath)
{
  std::vector<std::string> errors;
  const std::vector<std::string> files = getAllFilesInPath(configPath);
  for (const std::string &file : files)
  {
    if (!isValidTopicName(file.c_str()))
    {
      errors.push_back("[Error]:" + file + " is not a valid topic name.\n");
      continue;
    }
    const std::string subscription = getOneFileContentFromPath(configPath, file);
    if (subscription.empty())
    {
      errors.push_back("[Error]:" + file + " could not be validated or is empty.\n");
      continue;
    }
    std::istringstream iss(subscription);
    std::string line;
    int lineNumber = 0;
    const Sub *sub = nullptr;
    while (std::getline(iss, line))
    {
      sub = parseSubscription(line);
      lineNumber++;
      if (!sub)
      {
        errors.push_back("[Error]:" + file + " has an invalid subscription at line: " + std::to_string(lineNumber) + "\n");
        continue;
      }
      delete[] sub->command;
      delete sub;
    }
  }
  for (const std::string &error : errors)
  {
    std::cout << error;
  }
  return errors.empty() ? 0 : 1;
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
