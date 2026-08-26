#include <iostream>
#include <cstring>

#include "shriek.h"

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

void print_help() {
    std::cout << "Usage:\n"
           << "  shriek subscribe TOPIC COMMAND\n"
           << "  shriek unsubscribe TOPIC ID\n"
           << "  shriek update TOPIC ID COMMAND\n"
           << "  shriek emit TOPIC [MESSAGE]\n"
           << "  shriek list [TOPIC]\n"
           << "  shriek validate [TOPIC]\n"
           << "  shriek --help\n";
    std::cout << "\n";
    std::cout << "Silly Usage:\n"
           << "  shriek when TOPIC COMMAND\n"
           << "  shriek hush TOPIC ID\n"
           << "  shriek tune TOPIC ID COMMAND\n"
           << "  shriek at/about TOPIC [MESSAGE]\n"
           << "  shriek topics/victims [TOPIC]\n"
           << "  shriek clear-throat/soundcheck/mictesting123 [TOPIC]\n"
           << "  shriek --help\n";
}

int main(int argc, char *argv[], char *envp[])
{
  std::cout << "Shriek! " << VERSION << std::endl;
  argc--;
  argv++; // skip the program name
  const COM command = findCommand(argv[0]);
  switch (command)
  {
  case COM::SUBSCRIBE:
    // Handle subscribe command
    break;
  case COM::UNSUBSCRIBE:
    // Handle unsubscribe command
    break;
  case COM::UPDATE:
    // Handle update command
    break;
  case COM::EMIT:
    // Handle emit command
    break;
  case COM::LIST:
    // Handle list command
    break;
  case COM::VALIDATE:
    // Handle validate command
    break;

  default:
    print_help();
    break;
  }
  return 0;
}
