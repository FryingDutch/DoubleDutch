#pragma once
#include <boost/thread.hpp>
#include <string>
#include <vector>
#include "Lock.h"

namespace DoubleD
{
	class DDserver
	{
	private:
		static std::string m_server_name, m_crt_file_path, m_key_file_path, m_api_key;
		static int m_port, m_precision, m_threads;
		static bool m_is_https, m_error, m_isRunning, m_custom_api_key;

	private:
		static boost::mutex m_storageMutex;
		static std::vector<Lock> m_lockVector;

	private:
		//functions used to define the settings of the server
		static void m_errormsg(const char* message);
		static bool m_isDigit(std::string str);
		static void m_handlePrefixes(char* _argv[], int _argc);
		static void m_loadApiKey();

		//functions that are being used to define runtime situations
		static void m_startup();
		static std::string m_handleRequest(const unsigned int TIMEOUT, std::string lockName, const double LIFETIME);

		//dedicated thread
		static void m_checkLifetimes();

	public:
		DDserver();
		static void m_setAndBoot(int _argc, char* _argv[]);
	};
}
