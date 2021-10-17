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
		friend class BackUpRequest;

		static std::string server_name, crt_file_path, key_file_path, api_key;
		static int port, precision, threads;
		static bool error, isRunning, custom_api_key;

	public:
		static bool is_https;

	private:
		static boost::mutex storageMutex;
		static std::vector<Lock> lockVector;
		static std::vector<const char*> backup_addresses;

	private:
		//functions used to define the settings of the server
		static void errormsg(const char* message);
		static bool isDigit(std::string str);
		static const char* createStatusPayload();
		static void handleCommandLineArguments(char* _argv[], int _argc);
		static void loadApiKey();

		//functions that are being used to define runtime situations
		static void startup();
		static boost::optional<Lock> handleRequest(std::string lockName, const unsigned int TIMEOUT, const double LIFETIME);
		static boost::optional<Lock> getLock(std::string lockName, const double LIFETIME);

		//dedicated thread
		static void checkLifetimes();

	public:
		DDserver();
		static void setAndBoot(int _argc, char* _argv[]);
	};
}
