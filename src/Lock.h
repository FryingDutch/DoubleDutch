#ifndef LOCK_H
#define LOCK_H
#include <string>
#include <chrono>

namespace DoubleD
{
    class Lock
    {
    private:
        std::string m_name;
        std::string m_user_id;

    public:
        double m_lifeTime;
        std::chrono::_V2::system_clock::time_point m_start;

    public:
        Lock(std::string _name, std::string _id, double _lifeTime);
        bool m_expired();
        std::string m_getName();
        std::string m_getUser_id();
    };
}

#endif
