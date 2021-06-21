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
		static bool m_isRunning;
		static boost::mutex m_storageMutex;
		static std::vector<Lock> m_lockVector;

	private:
		static bool m_reqTimedout(unsigned int timeout, std::string lockName, const unsigned int PRECISION);
		static void m_checkLifetimes(const unsigned int PRECISION);
		static bool m_keyVerified(std::string key);

	public:
		DDserver();
		static void m_startup(const unsigned int PORTNUM, const unsigned int NUMOFTHREADS, const unsigned int PRECISION);		
	};
}

#endif
