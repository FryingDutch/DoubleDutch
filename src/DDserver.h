#pragma once
#ifndef DDserver_H
#define DDserver_H
#include <boost/thread.hpp>
#include <string>
#include <vector>
#include "Lock.h"

namespace DoubleD
{
	class DDserver
	{
	private:
		static boost::mutex m_storageMutex;
		static std::vector<Lock> m_lockVector;

	private:
		static bool m_reqTimedout(unsigned int timeout, std::string lockName);

	public:
		DDserver();
		static void m_startup(const unsigned int portNum);
		static void m_checkLifetimes();
	};
}

#endif
