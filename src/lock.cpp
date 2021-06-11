#include "Lock.h"

namespace DoubleD
{
	Lock::Lock(std::string _name, std::string _id, double _lifeTime)
		: m_name(_name), m_user_id(_id), m_lifeTime(_lifeTime)
	{
		m_start = std::chrono::high_resolution_clock::now();
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
