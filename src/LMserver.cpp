#include <vector>
#include <string>
#include <random>
#include "LMserver.h"
#include "crow.h"

LMserver::LMserver(unsigned int portNum)
{
  crow::SimpleApp app;
  CROW_ROUTE(app, "/")([]() { return "Welcome to LockManager"; });

  //need to figure out how to return an actual json
  CROW_ROUTE(app, "/status")
  ([&] {
    std::string tempString;
    for (unsigned int i = 0; i < this->lockVector.size(); i++) {
      tempString = tempString + "LOCK= " + this->lockVector[i].getName() + "\n" + "USER_ID= " + this->lockVector[i].getUser_id() + "\n\n";
    }
    return tempString;
  });

  //Getting the lock
  CROW_ROUTE(app, "/getLock/<string>")
  ([&](std::string lockName) {
      this->storageMutex.lock();
      for (long unsigned int i = 0; i < this->lockVector.size(); i++) {
        if (lockName == this->lockVector[i].getName()) {
          this->storageMutex.unlock();
          std::string ep = "false";
          return ep;
        }
      }

      std::string newID = this->createHash();
      Lock *tempLock = new Lock(lockName, newID);
      this->lockVector.push_back(*tempLock);
      delete tempLock;

      this->storageMutex.unlock();
      return newID;
  });

  //Releasing the lock
  CROW_ROUTE(app, "/releaseLock/<string>/<string>")
  ([&](std::string lockName, std::string user_id) {
    this->storageMutex.lock();
    for (long unsigned int i = 0; i < this->lockVector.size(); i++) {
      if (lockName == this->lockVector[i].getName() &&
          user_id == this->lockVector[i].getUser_id()) {
        this->lockVector.erase(this->lockVector.begin() + i);
        this->storageMutex.unlock();
        return "released";
      }
    }
    this->storageMutex.unlock();
    return "false";
  });

  app.port(portNum).multithreaded().run();
}

std::string LMserver::createHash() 
{
  static std::string str =
      "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  std::random_device rd;
  std::mt19937 generator(rd());
  std::shuffle(str.begin(), str.end(), generator);
  for (unsigned int i = 0; i < this->lockVector.size(); i++) {
    if (str.substr(0, 32) == this->lockVector[i].getUser_id()) {
      this->createHash();    
    }
  }
  return str.substr(0, 32);
}
