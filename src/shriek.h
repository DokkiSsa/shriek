#ifndef SHRIEK_H
#define SHRIEK_H

#include <ctype.h>

#define TOPIC_MAX 32

struct Sub
{
  int id;
  char *command;
};

struct Topic
{
  // 31 characters for the topic name, plus a null terminator
  char name[TOPIC_MAX];
  Sub *subs;
};

const bool isValidTopicName(const char *str)
{
  int length = 1;
  if (!str || !*str || !isalpha(*str))
    return false;
  while (*++str)
    if ((!isalnum(*str) && *str != '_') || length++ > TOPIC_MAX - 1)
      return false;
  return true;
}

inline const bool isValidId(const int id)
{
  return id > 0;
}

#endif // SHRIEK_H
