#ifndef LOCK_H
#define LOCK_H
#include <string>
#include <chrono>
#include <mutex>

namespace DoubleD
{
    class Lock
    {
    private:
        std::string m_name, m_session_token;
        double m_lifeTime;
        std::chrono::_V2::system_clock::time_point m_start;

    private:
        static std::mutex sessionTokenMutex;

    public:
        static std::vector<std::string> currentSessionTokens;

    private:
        std::string m_createToken(bool isFirstCycle = true);

    public:
        Lock(std::string _name, double _lifeTime);
        bool m_expired();
        double m_timeLeft();
        std::string m_getName();
        std::string m_getSessionToken();
        void m_removeSessionToken();
    };
}

#endif
