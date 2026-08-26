#ifndef SHRIEK_H
#define SHRIEK_H

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

#include <ctype.h>

#define TOPIC_MAX 32

namespace fs = std::filesystem;
typedef std::vector<std::string> errorList;

// ---------------------------
// data structures------------

struct Sub
{
  int id;
  std::string command;
};

struct Topic
{
  // 31 characters for the topic name, plus a null terminator
  std::string name;
  std::vector<Sub> subs;
};

// ---------------------------
// validation funcs-----------

inline const bool isValidId(const int id)
{
  return id > 0;
}

const bool isValidCommand(const char *command)
{
  const char *p = command;
  if (!p || !*p || !isgraph(*p))
    return false;
  for (p = command; *p; ++p)
    if (*p == '\n')
      return false;
  return true;
}

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

// ---------------------------
// file funcs-----------------

std::vector<std::string> getAllFilesInPath(const std::string &path)
{
  std::vector<std::string> files;
  for (const auto &entry : fs::directory_iterator(path))
    if (entry.is_regular_file())
      files.push_back(entry.path().filename().string());
  return files;
}

const std::string getOneFileContentFromPath(const std::string &path, const std::string &fileName)
{
  std::ifstream file(path + "/" + fileName);
  if (!file.is_open())
    return "";
  std::string content, line;
  while (std::getline(file, line))
    content += line + "\n";
  file.close();
  return content;
}

// ---------------------------
// parse funcs----------------

const Sub *parseOneSubscription(const std::string &subscription)
{
  size_t idEnd = subscription.find('\t');
  if (idEnd == std::string::npos)
    return nullptr;
  int id = std::stoi(subscription.substr(0, idEnd));
  std::string command = subscription.substr(idEnd + 1);
  if (!isValidId(id) || !isValidCommand(command.c_str()))
    return nullptr;
  return new Sub{id, command};
}

std::vector<Sub> parseSubscriptions(const std::string &subscriptions, errorList &errors)
{
  std::istringstream iss(subscriptions);
  std::string line;
  std::vector<Sub> subs;
  const Sub *sub = nullptr;
  int lineNumber = 0;
  while (std::getline(iss, line))
  {
    lineNumber++;
    sub = parseOneSubscription(line);
    if (!sub)
      errors.push_back("[Error]: Invalid subscription: (line:" + std::to_string(lineNumber) + ") " + line + "\n");
    subs.emplace_back(*sub);
  }
  if (errors.size())
    subs.clear();
  return subs;
}

const Topic *parseTopicFromFile(const std::string &path, const std::string &fileName, errorList &errors)
{
  Topic *topic = new Topic();
  topic->name = fileName;
  const std::string subscriptions = getOneFileContentFromPath(path, fileName);
  if (subscriptions.empty())
  {
    errors.push_back("[Error]:" + fileName + " could not be validated or is empty.\n");
    return nullptr;
  }
  topic->subs = parseSubscriptions(subscriptions, errors);
  if (topic->subs.empty())
  {
    errors.insert(errors.begin(), "[Error]:" + fileName + " has 1 or more invalid subscriptions.\n");
    return nullptr;
  }
  return topic;
}

#endif // SHRIEK_H
