#pragma once
#include <vector>
#include <optional>
#include "Lock.h"

namespace DoubleD
{
	struct LockManager
	{
	private:
		static std::vector<Lock> lockVector;

	private:
		static std::optional<Lock> getLock(std::string lockName, const double LIFETIME);

		//dedicated thread
		static void checkLifetimes();

		friend class DDserver;
		friend class BackupManager;
	};
}