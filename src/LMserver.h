#ifndef LMSERVER_H
#define LMSERVER_H

#include <boost/thread.hpp>
#include <string>
#include <vector>
#include "Lock.h"

class LMserver
{
 private:
  boost::mutex storageMutex;
  std::vector<Lock> lockVector;

 private:
  std::string createHash();

 public:
  LMserver(const unsigned int portNum);
};

#endif
