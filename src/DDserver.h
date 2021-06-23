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
		//functions used to define the settings of the server
		static void m_errormsg(const char* message);
		static bool m_isDigit(std::string str);
		static void m_handlePrefixes(char* _argv[], int& _port, int& _precision, int& _threads, bool& _is_https, bool& _error, int value);

		//functions that are being used to define runtime situations
		static void m_startup(const unsigned int PORTNUM, const unsigned int NUMOFTHREADS, const unsigned int PRECISION, const bool HTTPS);
		static bool m_reqTimedout(unsigned int timeout, std::string lockName, const unsigned int PRECISION);
		static void m_checkLifetimes(const unsigned int PRECISION);
		static bool m_keyVerified(std::string key);

	public:
		DDserver();
		static void m_setAndBoot(int _argc, char* _argv[]);
	};
}

#endif
