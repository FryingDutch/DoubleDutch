#pragma once
#ifndef LMSERVER_H
#define LMSERVER_H
#include <boost/thread.hpp>
#include <string>
#include <vector>
#include "Lock.h"

class LMserver
{
 public:
  static boost::mutex m_storageMutex;
  static std::vector<Lock> m_lockVector;

 private:
  static std::string m_createID();  
 
 public:
  LMserver();
  static void m_startup(const unsigned int portNum);
};

#endif
