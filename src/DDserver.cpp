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
    std::string DDserver::m_server_name = "DoubleDutch/v0.1";
    std::string DDserver::m_crt_file_path = "../SSL/certificate.crt";
    std::string DDserver::m_key_file_path = "../SSL/privateKey.key";
    std::string DDserver::m_api_key;

    int DDserver::m_port = 1;
    int DDserver::m_precision = 333;
    int DDserver::m_threads = std::thread::hardware_concurrency();

    bool DDserver::m_is_https = true;
    bool DDserver::m_error = false;
    bool DDserver::m_isRunning = true;
    bool DDserver::m_sibbling_key_needed = false;

    std::vector<Lock> DDserver::m_lockVector;
    boost::mutex DDserver::m_storageMutex;


    DDserver::DDserver()
    {}

    //setting functions
    void DDserver::m_errormsg(const char* message)
    {
        std::cout << "[ERROR]: " << message << "! Terminating...\n";
    }

    bool DDserver::m_isDigit(std::string str)
    {
        for (long unsigned int i = 0; i < str.length(); i++)
        {
            if (!std::isdigit(str[i]))
            {
                return false;
            }
        }
        return true;
    }

    void DDserver::m_handlePrefixes(char* _argv[], int _argc)
    {
        //if the argument count is higher then one (Need at least portnumber)
        //and the argument count is even (every prefix needs a value)
        //and the portnumber is an actual digit
        if (_argc > 1 && _argc % 2 == 0 && DDserver::m_isDigit(_argv[1]))
        {
            //set the port number
            DDserver::m_port = std::stoi(_argv[1]);

            //for every prefix check and assign the user input to the correct variable
            for (int i = 2; i < _argc; i += 2)
            {
                //if the value next to the prefix is a digit
                if (DDserver::m_isDigit(_argv[i+1]))
                {
                    switch (*_argv[i])
                    {
                    case 'p':
                        DDserver::m_precision = std::stoi(_argv[i+1]);
                        break;

                    case 't':
                        DDserver::m_threads = std::stoi(_argv[i+1]);
                        break;

                    case 'h':
                        if (std::stoi(_argv[i+1]) == 0)
                        {
                            DDserver::m_is_https = false;
                        }
                        else
                        {
                            DDserver::m_errormsg("h only takes 0 as an argument");
                            DDserver::m_error = true;
                        }
                        break;

                    default:
                        DDserver::m_errormsg("Not a valid prefix");
                        DDserver::m_error = true;
                        break;
                    }
                }

                else
                {
                    switch (*_argv[i])
                    {
                    case 'n':
                        DDserver::m_server_name = _argv[i+1];
                        break;

                    case 'c':
                        DDserver::m_crt_file_path = _argv[i+1];
                        DDserver::m_sibbling_key_needed = !DDserver::m_sibbling_key_needed;
                        break;

                    case 'k':
                        DDserver::m_key_file_path = _argv[i+1];
                        DDserver::m_sibbling_key_needed = !DDserver::m_sibbling_key_needed;
                        break;

                    default:
                        DDserver::m_errormsg("Not a valid prefix");
                        DDserver::m_error = true;
                        break;
                    }
                }
            }
        }

        else
        {
            DDserver::m_error = true;
            DDserver::m_errormsg("Not a valid input");
        }
    }

    void DDserver::m_loadApiKey()
    {
        std::ifstream file("../config.txt", std::ios::in);

        if (file.is_open())
        {
            std::string _str;
            std::getline(file, _str);
            file.close();

            if (_str == "")
            {
                DDserver::m_api_key = _str;                
            }

            else
            {
                DDserver::m_errormsg("Empty key file");
                DDserver::m_error = true;
            }
        }

        else
        {
            DDserver::m_errormsg("No key file found");
            DDserver::m_error = true;
        }
    }

    void DDserver::m_setAndBoot(int _argc, char* _argv[])
    {
        DDserver::m_loadApiKey();
        DDserver::m_handlePrefixes(_argv, _argc);

        if (DDserver::m_port > 0 && DDserver::m_threads > 0 && DDserver::m_precision > 0)
        {
            if (DDserver::m_error == false)
            {
                if (DDserver::m_sibbling_key_needed == false)
                {
                    DDserver::m_startup();
                }

                else
                {
                    DDserver::m_errormsg("Need both crt and key file");
                }
            }
        }

        else
        {
            DDserver::m_errormsg("Need [SIGNED INT] as argument");
        }
    }

    //runtime functions
    void DDserver::m_startup() 
    {
        crow::SimpleApp app;

        CROW_ROUTE(app, "/status")
            ([&] (const crow::request& req){
            crow::json::wvalue x;

            if (req.url_params.get("auth") == nullptr)
            {
                x["status"] = "no key";
                return crow::response(400, x);
            }
            else if(DDserver::m_api_key != req.url_params.get("auth"))
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
                        return crow::response(400, x);
                    }

                    else
                    {
                        if (DDserver::m_api_key != req.url_params.get("auth"))
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


                    if (DDserver::m_reqTimedout(timeout, lockName))
                    {
                        x["status"] = "timed out";
                        return crow::response(200, x);
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
                return crow::response(400, x);
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
            return crow::response(400, x);
                });

        std::thread th1(&DDserver::m_checkLifetimes);
        if (DDserver::m_is_https == true)
        {
            try
            {
                app.port(DDserver::m_port).server_name(DDserver::m_server_name).ssl_file(DDserver::m_crt_file_path, DDserver::m_key_file_path).concurrency(DDserver::m_threads).run();
            }

            catch (...)
            {
                DDserver::m_errormsg("Invalid key or crt file path");
            }
        }

        else
        {
            app.port(DDserver::m_port).server_name(DDserver::m_server_name).concurrency(DDserver::m_threads).run();
        }

        DDserver::m_isRunning = false;
        th1.join();
    }

    void DDserver::m_checkLifetimes()
    {
        while (DDserver::m_isRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(DDserver::m_precision));
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

    bool DDserver::m_reqTimedout(const unsigned int TIMEOUT, std::string lockName)
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
                std::this_thread::sleep_for(std::chrono::milliseconds(DDserver::m_precision));
            }            

        } while (difference.count() < TIMEOUT);
        return true;
    }    
}
