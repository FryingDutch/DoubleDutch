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
		static std::string server_name, crt_file_path, key_file_path, api_key;
		static int32_t port, precision, threads;
		static bool is_https, error, isRunning, custom_api_key;

	private:
		static boost::mutex storageMutex;
		static std::vector<Lock> lockVector;

	private:
		//functions used to define the settings of the server
		static void errormsg(const char* message);
		static bool isDigit(std::string str);
		static void handleCommandLineArguments(char* _argv[], int32_t _argc);
		static void loadApiKey();

		//functions that are being used to define runtime situations
		static void startup();
		static boost::optional<Lock> handleRequest(std::string lockName, const uint32_t TIMEOUT, const double LIFETIME);
		static boost::optional<Lock> getLock(std::string lockName, const double LIFETIME);

		//dedicated thread
		static void checkLifetimes();

	public:
		DDserver();
		static void setAndBoot(int _argc, char* _argv[]);
	};
}
