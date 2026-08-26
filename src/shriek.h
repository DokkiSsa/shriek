#ifndef SHRIEK_H
#define SHRIEK_H

#ifndef VERSION
  #define VERSION "1.0.0"
#endif

struct Sub {
  int id; 
  char *command;
};

struct Topic {
  // 31 characters for the topic name, plus a null terminator
  char name[32];
  Sub *subs;
};


#endif // SHRIEK_H
