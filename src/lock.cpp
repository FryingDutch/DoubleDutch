#include <random>
#include <algorithm>
#include "DDserver.h"
#include "Lock.h"

namespace DoubleD
{
	std::mutex Lock::sessionTokenMutex;
	std::vector<std::string> Lock::currentSessionTokens;

	Lock::Lock(std::string _name, double _lifeTime)
		: m_name(_name), m_lifeTime(_lifeTime)
	{
		m_start = std::chrono::high_resolution_clock::now();
		m_session_token = Lock::m_createToken();
	}

	void Lock::m_removeSessionToken()
	{
		sessionTokenMutex.lock();
		for (size_t i = 0; i < currentSessionTokens.size(); i++)
		{
			if (m_session_token == currentSessionTokens[i])
			{
				currentSessionTokens.erase(currentSessionTokens.begin() + i);
			}
		}
		sessionTokenMutex.unlock();
	}

	std::string Lock::m_createToken(bool isFirstCycle) // boolean is defaulted to true, argument call is only for internal use.
	{
		// This boolean allows the first cycle to lock and keep it locked for further cycles.
		// No. 2 and above cycles, the lock should stay active, so the data inside doesnt get changed in between recursive cycles.
		if (isFirstCycle)
			sessionTokenMutex.lock();

		static std::string str =
			"01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		static std::random_device rd;
		static std::mt19937 generator(rd());
		std::shuffle(str.begin(), str.end(), generator);
		std::string newToken = str.substr(0, 32);

		for (size_t i = 0; i < currentSessionTokens.size(); i++)
		{
			if (newToken == currentSessionTokens[i])
			{
				newToken = m_createToken(false);
			}
		}

		// only push and unlock when first cycle wants to return
		if (isFirstCycle)
		{
			currentSessionTokens.push_back(newToken.substr(0, 32));
			sessionTokenMutex.unlock();
		}			

		return newToken.substr(0, 32);
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
