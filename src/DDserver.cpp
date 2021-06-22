#define CROW_MAIN 
#define CROW_ENABLE_SSL
#include "crow.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include "DDserver.h"

namespace DoubleD
{
    std::vector<Lock> DDserver::m_lockVector;
    boost::mutex DDserver::m_storageMutex;
    bool DDserver::m_isRunning = true;

    DDserver::DDserver()
    {}

    void DDserver::m_startup(const unsigned int PORTNUM, const unsigned int NUMOFTHREADS, const unsigned int PRECISION) {

        crow::SimpleApp app;
        CROW_ROUTE(app, "/")([]() { return "Welcome to DoubleDutch"; });

        CROW_ROUTE(app, "/status")
            ([&] (const crow::request& req){
            crow::json::wvalue x;

            if (req.url_params.get("auth") == nullptr)
            {
                x["status"] = "no key";
                return crow::response(401, x);
            }
            else if(!DDserver::m_keyVerified(req.url_params.get("auth")))
            {
                x["status"] = "invalid key";
                return crow::response(401, x);
            }

            DDserver::m_storageMutex.lock();
            if (DDserver::m_lockVector.size() > 0)
            {
                for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++) {
                    x["status"][i]["name"] = DDserver::m_lockVector[i].m_getName();
                    x["status"][i]["token"] = DDserver::m_lockVector[i].m_getSessionToken();
                    x["status"][i]["remaining"] = DDserver::m_lockVector[i].m_timeLeft();
                }
                DDserver::m_storageMutex.unlock();
                return crow::response(200, x);
            }
            else 
            { 
                x["status"] = "empty"; 
                DDserver::m_storageMutex.unlock(); 
                return crow::response(200, x); 
            }
            });

        CROW_ROUTE(app, "/getLock")
            ([&](const crow::request& req)
            {
                    crow::json::wvalue x;
                    x["status"] = false;

                    std::string lockName;
                    double lifetime, timeout;

                    if (req.url_params.get("lockname") == nullptr || req.url_params.get("auth") == nullptr)
                    {
                        x["status"] = "invalid params";
                        return crow::response(401, x);
                    }

                    else
                    {
                        if (DDserver::m_keyVerified(req.url_params.get("auth")))
                        {
                            lockName = req.url_params.get("lockname");
                        }

                        else
                        {
                            x["status"] = "invalid key";
                            return crow::response(401, x);
                        }
                    }

                    if (req.url_params.get("lifetime") != nullptr)
                    {
                        lifetime = boost::lexical_cast<double>(req.url_params.get("lifetime"));
                    }

                    else
                    {
                        lifetime = 30.0f;
                    }

                    if (req.url_params.get("timeout") != nullptr)
                    {
                        timeout = boost::lexical_cast<double>(req.url_params.get("timeout"));
                    }

                    else
                    {
                        timeout = 0.0f;
                    }


                    if (DDserver::m_reqTimedout(timeout, lockName, PRECISION))
                    {
                        x["status"] = "timed out";
                        return crow::response(401, x);
                    }

                    else
                    {
                        Lock tempLock(lockName, lifetime);
                        DDserver::m_lockVector.push_back(tempLock);
                        DDserver::m_storageMutex.unlock();
                        x["status"] = tempLock.m_getSessionToken();
                        return crow::response(200, x);
                    }
            });

        // Releasing the lock
        CROW_ROUTE(app, "/releaseLock")
            ([&](const crow::request& req) {
            std::string lockName, user_id;
            crow::json::wvalue x;
            x["status"] = false;
            if (req.url_params.get("lockname") == nullptr || req.url_params.get("key") == nullptr)
            {
                x["status"] = "invalid params";
                return crow::response(401, x);
            }

            else
            {
                lockName = req.url_params.get("lockname");
                user_id = req.url_params.get("key");
            }

            DDserver::m_storageMutex.lock();
            for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++) {
                
                if (lockName == DDserver::m_lockVector[i].m_getName() &&
                    user_id == DDserver::m_lockVector[i].m_getSessionToken()) {
                    DDserver::m_lockVector.erase(DDserver::m_lockVector.begin() + i);
                    DDserver::m_lockVector.shrink_to_fit();
                    DDserver::m_storageMutex.unlock();
                    x["status"] = "released";
                    return crow::response(200, x);
                }
            }
            DDserver::m_storageMutex.unlock();
            x["status"] = "no match";
            return crow::response(401, x);
                });

        std::thread th1(&DDserver::m_checkLifetimes, PRECISION);
        app.port(PORTNUM).ssl_file("../SSL/certificate.crt", "../SSL/privateKey.key").concurrency(NUMOFTHREADS).run();
        DDserver::m_isRunning = false;
        th1.join();
    }

    void DDserver::m_checkLifetimes(const unsigned int PRECISION)
    {
        while (DDserver::m_isRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(PRECISION));
            DDserver::m_storageMutex.lock();
            for (unsigned int i = 0; i < DDserver::m_lockVector.size(); i++)
            {
                if (DDserver::m_lockVector[i].m_expired())
                {
                    DDserver::m_lockVector.erase(DDserver::m_lockVector.begin() + i);
                    DDserver::m_lockVector.shrink_to_fit();
                }
            }
            DDserver::m_storageMutex.unlock();
        }
    }

    bool DDserver::m_reqTimedout(const unsigned int TIMEOUT, std::string lockName, const unsigned int PRECISION)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        auto currentTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> difference = currentTime - startTime;
        do
        {
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

                else if (i == DDserver::m_lockVector.size() - 1)
                {
                    return false;
                }
            }
            DDserver::m_storageMutex.unlock();

            currentTime = std::chrono::high_resolution_clock::now();
            difference = currentTime - startTime;

            if (difference.count() < TIMEOUT)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(PRECISION));
            }            

        } while (difference.count() < TIMEOUT);
        return true;
    }

    bool DDserver::m_keyVerified(std::string key)
    {
        std::ifstream file("../config.txt", std::ios::in);

        if (file.is_open())
        {
            std::string tempStr;
            std::getline(file, tempStr);
            file.close();

            return tempStr == key;
        }

        else
        {
            return false;
        }
    }
}
