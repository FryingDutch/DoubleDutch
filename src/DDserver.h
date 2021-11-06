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

	private:
		//functions that are being used to define runtime situations
		static void startup();
		static std::optional<Lock> handleRequest(std::string lockName, const uint32_t TIMEOUT, const double LIFETIME);		

	public:
		DDserver();
		static void setAndBoot(int _argc, char* _argv[]);
	
		friend class LockManager;
	};
}
