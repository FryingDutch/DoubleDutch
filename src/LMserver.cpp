#include <vector>
#include <string>
#include <random>
#include "LMserver.h"
#include "crow.h"

namespace DoubleD
{
    bool LMserver::isRunning = true;
    std::vector<Lock> LMserver::m_lockVector;
    boost::mutex LMserver::m_storageMutex;

    LMserver::LMserver()
    {}

    void LMserver::m_startup(const unsigned int portNum) {

        std::cout << std::this_thread::get_id() << "\n";
        crow::SimpleApp app;
        CROW_ROUTE(app, "/")([]() { return "Welcome to LockManager"; });

        // need to figure out how to return actual json
        CROW_ROUTE(app, "/status")
            ([&] {
            std::string tempString;
            for (long unsigned int i = 0; i < LMserver::m_lockVector.size(); i++) {
                tempString = tempString + "LOCK= " + LMserver::m_lockVector[i].m_getName() +
                    "\n" + "USER_ID= " + LMserver::m_lockVector[i].m_getUser_id() +
                    "\n\n";
            }
            return tempString;
                });

        // Getting the lock with a custom lock lifetime
        CROW_ROUTE(app, "/getLock/<string>/<int>")
            ([&](std::string lockName, unsigned int lockLife) {
            LMserver::m_storageMutex.lock();
            for (long unsigned int i = 0; i < LMserver::m_lockVector.size(); i++) {
                if (lockName == LMserver::m_lockVector[i].m_getName()) {
                    LMserver::m_storageMutex.unlock();
                    std::string ep = "false";
                    return ep;
                }
            }
            std::string newID = LMserver::m_createID();
            Lock* tempLock = new Lock(lockName, newID, lockLife);
            LMserver::m_lockVector.push_back(*tempLock);
            delete tempLock;

            LMserver::m_storageMutex.unlock();
            return newID;
                });

        // Getting the lock with a default lock lifetime (30s)
        CROW_ROUTE(app, "/getLock/<string>")
            ([&](std::string lockName) {
            LMserver::m_storageMutex.lock();
            for (long unsigned int i = 0; i < LMserver::m_lockVector.size(); i++) {
                if (lockName == LMserver::m_lockVector[i].m_getName()) {
                    LMserver::m_storageMutex.unlock();
                    std::string ep = "false";
                    return ep;
                }
            }
            std::string newID = LMserver::m_createID();
            Lock* tempLock = new Lock(lockName, newID, 30.0f);
            LMserver::m_lockVector.push_back(*tempLock);
            delete tempLock;

            LMserver::m_storageMutex.unlock();
            return newID;
                });

        // Releasing the lock
        CROW_ROUTE(app, "/releaseLock/<string>/<string>")
            ([&](std::string lockName, std::string user_id) {
            LMserver::m_storageMutex.lock();
            for (long unsigned int i = 0; i < LMserver::m_lockVector.size(); i++) {
                if (lockName == LMserver::m_lockVector[i].m_getName() &&
                    user_id == LMserver::m_lockVector[i].m_getUser_id()) {
                    LMserver::m_lockVector.erase(LMserver::m_lockVector.begin() + i);
                    LMserver::m_lockVector.shrink_to_fit();
                    LMserver::m_storageMutex.unlock();
                    return "released";
                }
            }
            LMserver::m_storageMutex.unlock();
            return "false";
                });

        app.port(portNum).multithreaded().run();
        LMserver::LMserver::isRunning = false;
    }

    std::string LMserver::m_createID()
    {
        static std::string str =
            "01234567890ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        std::random_device rd;
        std::mt19937 generator(rd());
        std::shuffle(str.begin(), str.end(), generator);
        for (long unsigned int i = 0; i < LMserver::m_lockVector.size(); i++) {
            if (str.substr(0, 32) == LMserver::m_lockVector[i].m_getUser_id()) {
                return LMserver::m_createID();
            }
        }
        return str.substr(0, 32);
    }

    void LMserver::m_checkLifetimes()
    {
        for (;;) {
            LMserver::m_storageMutex.lock();
            if (LMserver::m_lockVector.size() > 0)
            {
                for (unsigned int i = 0; i < LMserver::m_lockVector.size(); i++)
                {
                    if (LMserver::m_lockVector[i].m_expired())
                    {
                        LMserver::m_lockVector.erase(LMserver::m_lockVector.begin() + i);
                        LMserver::m_lockVector.shrink_to_fit();
                    }
                }
            }
            LMserver::m_storageMutex.unlock();
        }
    }
}
