#ifndef SHRIEK_H
#define SHRIEK_H

#include <filesystem>
#include <string>
#include <vector>

#include <ctype.h>

#define TOPIC_MAX 32

namespace fs = std::filesystem;

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

std::vector<std::string> getAllFilesInPath(const std::string &path)
{
  std::vector<std::string> files;
  for (const auto &entry : fs::directory_iterator(path))
    if (entry.is_regular_file())
      files.push_back(entry.path().string());
  return files;
}

std::string getOneFileInPath(const std::string &path, const std::string &fileName)
{
  for (const auto &entry : fs::directory_iterator(path))
    if (entry.is_regular_file())
    {
      const auto &filePath = entry.path();
      if (filePath.filename().string() == fileName)
        return filePath.string();
    }
  return {}; // Return empty string if no file found
}

#endif // SHRIEK_H
