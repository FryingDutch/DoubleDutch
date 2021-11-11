#pragma once
#include <string>
#include <chrono>

namespace DoubleD
{
    class Lock
    {
    private:
        std::string m_name, m_session_token;
        double m_lifeTime;
        std::chrono::_V2::system_clock::time_point m_start;

    private:
        std::string m_createToken();

    public:
        Lock(std::string _name, double _lifeTime);
        Lock(std::string _name, double _remainingTime, std::string _session_token);
        bool m_expired();
        double m_timeLeft();
        std::string m_getName();
        std::string m_getSessionToken();
    };
}
