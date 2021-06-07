#ifndef LOCK_H
#define LOCK_H
#include <string>

class Lock
{
 private:
  std::string name;
  std::string user_id;

 public:
  Lock(std::string _name, std::string _id) : name(_name), user_id(_id){};
  std::string getName() { return this->name; }
  std::string getUser_id() { return this->user_id; }
};
#endif
