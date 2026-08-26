#include <iostream>
#include <cstring>
#include <cstdlib>

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

int subscribe(const char *topic, const char *command)
{
  if (!isValidTopicName(topic))
  {
    std::cerr << "Invalid topic name: " << topic << std::endl;
    return 1;
  }
  // Implement subscription logic here
  std::cout << "Subscribed to topic: " << topic << " with command: " << command << std::endl;
  return 0;
}

int unsubscribe(const char *topic, int id)
{
  if (!isValidTopicName(topic))
  {
    std::cerr << "Invalid topic name: " << topic << std::endl;
    return 1;
  }
  if (!isValidId(id))
  {
    std::cerr << "Invalid subscription ID: " << id << std::endl;
    return 1;
  }
  // Implement unsubscription logic here
  std::cout << "Unsubscribed from topic: " << topic << " with ID: " << id << std::endl;
  return 0;
}

int update(const char *topic, int id, const char *command)
{
  if (!isValidTopicName(topic))
  {
    std::cerr << "Invalid topic name: " << topic << std::endl;
    return 1;
  }
  if (!isValidId(id))
  {
    std::cerr << "Invalid subscription ID: " << id << std::endl;
    return 1;
  }
  // Implement update logic here
  std::cout << "Updated subscription for topic: " << topic << " with ID: " << id << " to command: " << command << std::endl;
  return 0;
}

int emit(const char *topic, const char *message)
{
  if (!isValidTopicName(topic))
  {
    std::cerr << "Invalid topic name: " << topic << std::endl;
    return 1;
  }
  // Implement emit logic here
  std::cout << "Emitted message to topic: " << topic << " with message: " << (message ? message : "(no message)") << std::endl;
  return 0;
}

int list(const char *topic)
{
  if (topic && !isValidTopicName(topic))
  {
    std::cerr << "Invalid topic name: " << topic << std::endl;
    return 1;
  }
  // Implement list logic here
  std::cout << "Listing subscriptions for topic: " << (topic ? topic : "(all topics)") << std::endl;
  return 0;
}

int validate()
{
  // Implement validation logic here
  std::cout << "Validation successful." << std::endl;
  return 0;
}

int main(int argc, char *argv[])
{
  argc--; argv++; // skip the program name
  if (argc < 1)
  {
    print_help();
    return 1;
  }
  const COM command = findCommand(argv[0]);
  argc--; argv++; // skip the command
  switch (command)
  {
  
  case COM::SUBSCRIBE:
    // Handle subscribe command
    if (argc < 2)
    {
      std::cerr << "Error: subscribe command requires a topic and a command." << std::endl;
      return 1;
    }
    return subscribe(argv[0], argv[1]);
    break;
  case COM::UNSUBSCRIBE:
    // Handle unsubscribe command
    if (argc < 2)
    {
      std::cerr << "Error: unsubscribe command requires a topic and an ID." << std::endl;
      return 1;
    }
    return unsubscribe(argv[0], std::stoi(argv[1]));
    break;
  case COM::UPDATE:
    // Handle update command
    if (argc < 3)
    {
      std::cerr << "Error: update command requires a topic, an ID, and a new command." << std::endl;
      return 1;
    }
    return update(argv[0], std::stoi(argv[1]), argv[2]);
    break;
  case COM::EMIT:
    // Handle emit command
    if (argc < 2)
    {
      std::cerr << "Error: emit command requires a topic and a message." << std::endl;
      return 1;
    }
    return emit(argv[0], argv[1]);
    break;
  case COM::LIST:
    return list(argv[0] ? argv[0] : nullptr);
    break;
  case COM::VALIDATE:
    return validate();
    break;

  default:
    print_help();
    break;
  }
  return 0;
}
