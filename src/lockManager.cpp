#include <vector>
#include <optional>
#include "Lock.h"
#include "Settings.h"
#include "DDserver.h"
#include "LockManager.h"

namespace DoubleD
{
    std::vector<Lock> LockManager::lockVector;
    // returns a Lock if a Lock can be acquired, otherwise returns boost::none.
    std::optional<Lock> LockManager::getLock(std::string lockName, const double LIFETIME) 
    {
        // determine whether the lock with <lockName> is free/available
        bool _free{ true };
        DDserver::storageMutex.lock();
        for (size_t i = 0; i < lockVector.size(); i++)
        {
            if (lockName == lockVector[i].m_getName() && !lockVector[i].m_expired())
            {
                _free = false;
                break;
            }
        }

        // insert a Lock if <lockName> is free/available
        std::optional<Lock> _lock;
        if (_free) {
            _lock = Lock(lockName, LIFETIME);
            lockVector.push_back(_lock.value());
        }
        DDserver::storageMutex.unlock();
        return _lock;
    }

    void LockManager::checkLifetimes()
    {
        while (Settings::isRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(Settings::precision));
            DDserver::storageMutex.lock();
            for (size_t i = 0; i < lockVector.size(); i++)
            {
                if (lockVector[i].m_expired())
                {
                    lockVector.erase(lockVector.begin() + i);
                    lockVector.shrink_to_fit();
                }
            }
            DDserver::storageMutex.unlock();
        }
    }
}