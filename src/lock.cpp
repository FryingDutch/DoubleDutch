#include <random>
#include "DDserver.h"
#include "Lock.h"

namespace DoubleD
{
	Lock::Lock(std::string _name, double _lifeTime)
		: m_name(_name), m_lifeTime(_lifeTime)
	{
		m_start = std::chrono::high_resolution_clock::now();
		m_user_id = Lock::m_createID();
	}

	std::string Lock::m_createID()
	{
		static std::string str =
			"01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		std::random_device rd;
		std::mt19937 generator(rd());
		std::shuffle(str.begin(), str.end(), generator);

		return str.substr(0, 32);
	}

	bool Lock::m_expired()
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> difference = currentTime - m_start;
		if (difference.count() > this->m_lifeTime)
		{ return true; }

		else
		{ return false;	}
	}

	std::string Lock::m_getName() { return this->m_name; }
	std::string Lock::m_getUser_id() { return this->m_user_id; }
}
