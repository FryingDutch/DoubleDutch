#pragma once
#include <thread>
#include <mutex>
#include <string>
#include <vector>
#include <optional>
#include "Lock.h"

namespace DoubleD
{
	class DDserver
	{
	private:
		static std::mutex storageMutex;
		static std::vector<Lock> lockVector;

	private:
		//functions that are being used to define runtime situations
		static void startup();
		static std::optional<Lock> handleRequest(std::string lockName, const uint32_t TIMEOUT, const double LIFETIME);
		static std::optional<Lock> getLock(std::string lockName, const double LIFETIME);

		//dedicated thread
		static void checkLifetimes();

	public:
		DDserver();
		static void setAndBoot(int _argc, char* _argv[]);
	};
}
