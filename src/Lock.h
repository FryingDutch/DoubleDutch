#ifndef LOCK_H
#define LOCK_H
#include <string>

struct Lock
{
 public:
  std::string name;
  unsigned int user_id;

 public:
  Lock(std::string _name, unsigned int _id) : name(_name), user_id(_id){};
};
#endif
