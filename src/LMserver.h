#pragma once
#ifndef LMSERVER_H
#define LMSERVER_H
#include <boost/thread.hpp>
#include <string>
#include <vector>
#include "Lock.h"

class LMserver
{
 private:
  static unsigned int m_portNum;
  static boost::mutex m_storageMutex;

 private:
  static std::string m_createID();  
  static std::vector<Lock> m_lockVector;

 public:
  static void m_startup();
 
 public:
  LMserver();
};

#endif
