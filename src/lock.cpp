#include <random>
#include <algorithm>
#include "DDserver.h"
#include "Lock.h"

namespace DoubleD
{
	Lock::Lock(std::string _name, double _lifeTime)
		: m_name(_name), m_lifeTime(_lifeTime)
	{
		m_start = std::chrono::high_resolution_clock::now();
		m_session_token = Lock::m_createToken();
	}

	Lock::Lock(std::string _name, double _remainingTime, std::string _session_token)
		: m_name(_name), m_session_token(_session_token), m_lifeTime(_remainingTime)
	{
		m_start = std::chrono::high_resolution_clock::now();
	}

	std::string Lock::m_createToken()
	{
		static std::string str =
			"01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		static std::random_device rd;
		static std::mt19937 generator(rd());
		std::shuffle(str.begin(), str.end(), generator);

		return str.substr(0, 32);
	}

	bool Lock::m_expired()
	{
		return this->m_timeLeft() < 0;
	}

	double Lock::m_timeLeft() 
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> difference = currentTime - m_start;

		return this->m_lifeTime - difference.count();
	}
	std::string Lock::m_getName() { return this->m_name; }
	std::string Lock::m_getSessionToken() { return this->m_session_token; }
}
