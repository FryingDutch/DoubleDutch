#pragma once
#include <vector>
#include <optional>
#include "Lock.h"

namespace DoubleD
{
	struct LockManager
	{
	public:
		static std::vector<Lock> lockVector;

	private:
		static std::optional<Lock> getLock(std::string lockName, const double LIFETIME);

		//dedicated thread
		static void checkLifetimes();

		friend class DDserver;
	};
}