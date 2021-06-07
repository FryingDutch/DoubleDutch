#include <vector>
#include "LMserver.h"
#include "crow.h"

LMserver::LMserver(unsigned int portNum)
{
  crow::SimpleApp app;
  CROW_ROUTE(app, "/")([]() { return "Hello LockWorld"; });

  //need to figure out how to return a full json template in this
  CROW_ROUTE(app, "/status/json")
  ([&] {
    crow::json::wvalue x;
    for (unsigned int i = 0; i < lockVector.size(); i++) {
      x[lockVector[i].name] = "NAME"; 
      x[lockVector[i].user_id] = "USER_ID";
    }
    return x;
  });

  CROW_ROUTE(app, "/getLock/<string>/<int>")
  ([&](std::string lockName, unsigned int user_id) {
    if (!this->getLock(lockName, user_id)) {
      return "false";
    }

    else {
      return "true";
    }
  });

  CROW_ROUTE(app, "/releaseLock/<string>/<int>")
  ([&](std::string lockName, unsigned int user_id) {
    if (this->releaseLock(lockName, user_id)) {
      return "Released";
    }

    else {
      return "NF";
    }
  });

  app.port(portNum).multithreaded().run();
}

bool LMserver::getLock(std::string lockName, unsigned int user_id)
{
  this->storageMutex.lock();
  for (long unsigned int i = 0; i < this->lockVector.size(); i++) {
    if (lockName == lockVector[i].name) {
      this->storageMutex.unlock(); 
      return false;
    }
  }

  Lock *tempLock = new Lock(lockName, user_id);
  this->lockVector.push_back(*tempLock);
  delete tempLock;

  this->storageMutex.unlock();
  return true;
}

bool LMserver::releaseLock(std::string lockName, unsigned int user_id)
{
  this->storageMutex.lock();
  for (long unsigned int i = 0; i < this->lockVector.size(); i++) {
    if (lockName == lockVector[i].name && user_id == lockVector[i].user_id) {
      this->lockVector.erase(this->lockVector.begin() + i);
      this->storageMutex.unlock();
      return true;
    }
  }
  this->storageMutex.unlock();
  return false;
}
