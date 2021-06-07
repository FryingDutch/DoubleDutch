#ifndef LOCK_H
#define LOCK_H
#include <string>

class Lock
{
 private:
  std::string name;
  unsigned int user_id;

 public:
  Lock(std::string _name, unsigned int _id) : name(_name), user_id(_id){};
  std::string getName() { return this->name; }
  unsigned int getUser_id() { return this->user_id; }
};
#endif
