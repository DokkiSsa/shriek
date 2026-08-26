#include <iostream>

struct Sub {
  char id[4];
  char *command;
};

struct Topic {
  char name[32];
  Sub *subs;
};

int main(int argc, char* argv[]) {
    std::cout << "Shriek!" << std::endl;
    return 0;
}

