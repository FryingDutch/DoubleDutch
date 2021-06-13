#include <vector>
#include <string>
#include <random>
#include "DDserver.h"
#include "crow.h"
#include <iostream>

namespace DoubleD
{
    std::vector<Lock> DDserver::m_lockVector;
    boost::mutex DDserver::m_storageMutex;

    DDserver::DDserver()
    {}

    void DDserver::m_startup(const unsigned int portNum) {

        std::cout << std::this_thread::get_id() << "\n";
        crow::SimpleApp app;
        CROW_ROUTE(app, "/")([]() { return "Welcome to LockManager"; });

        // need to figure out how to return actual json
        CROW_ROUTE(app, "/status")
            ([&] {
            std::string tempString;
            for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++) {
                tempString = tempString + "LOCK= " + DDserver::m_lockVector[i].m_getName() +
                    "\n" + "USER_ID= " + DDserver::m_lockVector[i].m_getUser_id() +
                    "\n\n";
            }
            return tempString;
                });

        // Getting the lock with a custom lock lifetime and a request timeout
        CROW_ROUTE(app, "/getLock/<int>/<string>/<int>")
            ([&](unsigned int timeout, std::string lockName, unsigned int lockLife) {
            DDserver::m_storageMutex.lock();
            for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++) {
                if (lockName == DDserver::m_lockVector[i].m_getName()) {
                    if(DDserver::m_reqTimedout(timeout, lockName))
                    {
                        DDserver::m_storageMutex.unlock();
                        std::string ep = "false";
                        return ep;
                    }
                }
            }
            Lock tempLock(lockName, lockLife);
            DDserver::m_lockVector.push_back(tempLock);

            DDserver::m_storageMutex.unlock();
            return tempLock.m_getUser_id();
                });

        // Getting the lock with a custom lock lifetime
        CROW_ROUTE(app, "/getLock/<string>/<int>")
            ([&](std::string lockName, unsigned int lockLife) {
            DDserver::m_storageMutex.lock();
            for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++) {
                if (lockName == DDserver::m_lockVector[i].m_getName()) {
                    DDserver::m_storageMutex.unlock();
                    std::string ep = "false";
                    return ep;
                }
            }
            Lock tempLock(lockName, lockLife);
            DDserver::m_lockVector.push_back(tempLock);

            DDserver::m_storageMutex.unlock();
            return tempLock.m_getUser_id();
                });

        // Getting the lock with a default lock lifetime (30s)
        CROW_ROUTE(app, "/getLock/<string>")
            ([&](std::string lockName) {
            DDserver::m_storageMutex.lock();
            for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++) {
                if (lockName == DDserver::m_lockVector[i].m_getName()) {
                    DDserver::m_storageMutex.unlock();
                    std::string ep = "false";
                    return ep;
                }
            }
            Lock tempLock(lockName, 30.0f);
            DDserver::m_lockVector.push_back(tempLock);

            DDserver::m_storageMutex.unlock();
            return tempLock.m_getUser_id();
                });

        // Releasing the lock
        CROW_ROUTE(app, "/releaseLock/<string>/<string>")
            ([&](std::string lockName, std::string user_id) {
            DDserver::m_storageMutex.lock();
            for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++) {
                if (lockName == DDserver::m_lockVector[i].m_getName() &&
                    user_id == DDserver::m_lockVector[i].m_getUser_id()) {
                    DDserver::m_lockVector.erase(DDserver::m_lockVector.begin() + i);
                    DDserver::m_lockVector.shrink_to_fit();
                    DDserver::m_storageMutex.unlock();
                    return "released";
                }
            }
            DDserver::m_storageMutex.unlock();
            return "false";
                });

        app.port(portNum).multithreaded().run();
    }

    void DDserver::m_checkLifetimes()
    {
        for (;;) 
        {
            DDserver::m_storageMutex.lock(); 
            {
                for (unsigned int i = 0; i < DDserver::m_lockVector.size(); i++)
                {
                    if (DDserver::m_lockVector[i].m_expired())
                    {
                        DDserver::m_lockVector.erase(DDserver::m_lockVector.begin() + i);
                        DDserver::m_lockVector.shrink_to_fit();
                    }
                }
            }
            DDserver::m_storageMutex.unlock();
        }        
    }

    bool DDserver::m_reqTimedout(unsigned int timeout, std::string lockName)
    {
        DDserver::m_storageMutex.unlock();

        auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> difference = currentTime - startTime;
        while (difference.count() < timeout)
        {
            currentTime = std::chrono::high_resolution_clock::now();
            difference = currentTime - startTime;

            DDserver::m_storageMutex.lock();
            if (DDserver::m_lockVector.size() == 0)
            {
                return false;
            }

            for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++)
            {
                if (lockName == DDserver::m_lockVector[i].m_getName())
                {

                    break;
                }

                //statement doesnt seem to be catching
                else if (i == DDserver::m_lockVector.size() - 1)
                {
                    return false;
                }
            }
            DDserver::m_storageMutex.unlock();
        } 
        DDserver::m_storageMutex.lock();
        return true;
    }    
}
