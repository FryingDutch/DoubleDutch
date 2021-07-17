#define CROW_MAIN 
#define CROW_ENABLE_SSL
#include "crow.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include "DDserver.h"
#include <boost/optional.hpp>
#include <boost/none.hpp>

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
    bool DDserver::m_custom_api_key = false;

    std::vector<Lock> DDserver::m_lockVector;
    boost::mutex DDserver::m_storageMutex;


    DDserver::DDserver()
    {}

    //setting functions
    void DDserver::m_errormsg(const char* message)
    {
        DDserver::m_error = true;
        std::cerr << "[ERROR]: " << message << "! Terminating...\n";
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

    void DDserver::m_handleCommandLineArguments(char* _argv[], int _argc)
    {
        //if the argument count is higher then one (Need at least portnumber)
        //and the argument count is even (every prefix needs a value)
        //and the portnumber is an actual digit
        if (_argc > 1 && _argc % 2 == 0 && DDserver::m_isDigit(_argv[1]))
        {
            //set the port number
            DDserver::m_port = std::stoi(_argv[1]);

            //for every flag check and assign the user input to the correct variable
            for (int i = 2; i < _argc; i += 2)
            {
                //if the value next to the flag is a digit
                if (DDserver::m_isDigit(_argv[i + 1]))
                {
                    switch (*_argv[i])
                    {
                    case 'p':
                        DDserver::m_precision = std::stoi(_argv[i + 1]);
                        break;

                    case 't':
                        DDserver::m_threads = std::stoi(_argv[i + 1]);
                        break;

                    case 'h':
                        if (std::stoi(_argv[i + 1]) == 0)
                        {
                            DDserver::m_is_https = false;
                        }
                        else
                        {
                            DDserver::m_errormsg("h only takes 0 as an argument");
                        }
                        break;

                    default:
                        DDserver::m_errormsg("Not a valid command-line flag");
                        break;
                    }
                }

                else
                {
                    switch (*_argv[i])
                    {
                    case 'n':
                        DDserver::m_server_name = _argv[i + 1];
                        break;

                    case 'c':
                        DDserver::m_crt_file_path = DDserver::m_crt_file_path.substr(0, 7) + _argv[i + 1];
                        break;

                    case 'k':
                        DDserver::m_key_file_path = DDserver::m_key_file_path.substr(0, 7) + _argv[i + 1];
                        break;

                    case 'a':
                        DDserver::m_api_key = _argv[i + 1];
                        DDserver::m_custom_api_key = true;
                        break;

                    default:
                        DDserver::m_errormsg("Not a valid flag");
                        break;
                    }
                }
            }
        }

        else
        {
            DDserver::m_errormsg("Not a valid input");
        }
    }

    // reads the API key from a file, and assigns its string value to DDserver::m_api_key
    void DDserver::m_loadApiKey()
    {
        std::ifstream file("../config.txt", std::ios::in);
        if (file.is_open())
        {
            std::string _str;
            std::getline(file, _str);
            file.close();

            if (_str != "")
            {
                DDserver::m_api_key = _str;
            }

            else
            {
                DDserver::m_errormsg("Empty key file");
            }
        }

        else
        {
            DDserver::m_errormsg("No API-key file found");
        }
    }


    void DDserver::m_setAndBoot(int _argc, char* _argv[])
    {

        DDserver::m_handleCommandLineArguments(_argv, _argc);
        if(DDserver::m_custom_api_key == false) DDserver::m_loadApiKey();


        if (DDserver::m_port > 0 && DDserver::m_threads > 0 && DDserver::m_precision > 0)
        {
            if (DDserver::m_error == false)
            {
                DDserver::m_startup();
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
            ([&](const crow::request& req) {
            
            crow::json::wvalue x;
            x["servername"] = DDserver::m_server_name;

            if (req.url_params.get("auth") == nullptr)
            {
                x["status"] = "no key";
                return crow::response(400, x);
            }
            else if (DDserver::m_api_key != req.url_params.get("auth"))
            {
                x["status"] = "invalid key";
                return crow::response(401, x);
            }

            x["status"] = "ok";

            // Create an empty vector.
            // Crow will convert this to a JSON list.
            // The list will be overwritten if the server contains locks.
            std::vector<std::string> jsonList;
            x["locks"] = jsonList;

            DDserver::m_storageMutex.lock();
            for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++) {
                x["locks"][i]["lockname"] = DDserver::m_lockVector[i].m_getName();
                x["locks"][i]["sessiontoken"] = DDserver::m_lockVector[i].m_getSessionToken();
                x["locks"][i]["remaining"] = DDserver::m_lockVector[i].m_timeLeft();
            }
            DDserver::m_storageMutex.unlock();
            return crow::response(200, x);
        });

        CROW_ROUTE(app, "/getlock")
            ([&](const crow::request& req)
                {
                    crow::json::wvalue x;
                    x["servername"] = DDserver::m_server_name;

                    // lockName
                    std::string lockName;
                    if (req.url_params.get("lockname") == nullptr) 
                    {
                        x["error"] = "no lockname supplied";
                        return crow::response(400, x);
                    }
                    lockName = req.url_params.get("lockname");

                    // auth
                    if (req.url_params.get("auth") == nullptr)
                    {
                        x["error"] = "no api key supplied";
                        return crow::response(401, x);
                    }
                    if (DDserver::m_api_key != req.url_params.get("auth"))
                    {
                        x["error"] = "invalid api key";
                        return crow::response(401, x);
                    }

                    // lifetime and timeout
                    double lifetime, timeout;
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
                    
                    boost::optional<Lock> lock = DDserver::m_handleRequest(lockName, timeout, lifetime);
                    x["sessiontoken"] = lock ? lock.get().m_getSessionToken() : nullptr;
                    x["lockacquired"] = lock ? true : false;
                    x["lockname"] = lockName;
                    return crow::response(200, x);
                });

        // Releasing the lock
        CROW_ROUTE(app, "/releaselock").methods("DELETE"_method)
            ([&](const crow::request& req) {
            std::string lockName, session_token;
            crow::json::wvalue x;
            x["servername"] = DDserver::m_server_name;

            if (req.url_params.get("lockname") == nullptr || req.url_params.get("token") == nullptr)
            {
                x["error"] = "invalid params";
                return crow::response(400, x);
            }

            else
            {
                lockName = req.url_params.get("lockname");
                session_token = req.url_params.get("token");
            }

            bool released {false};
            DDserver::m_storageMutex.lock();
            for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++) {

                if (lockName == DDserver::m_lockVector[i].m_getName() && 
                session_token == DDserver::m_lockVector[i].m_getSessionToken()) 
                { 
                    DDserver::m_lockVector.erase(DDserver::m_lockVector.begin() + i);
                    DDserver::m_lockVector.shrink_to_fit();
                    released = true;
                    break;
                }
            }
            DDserver::m_storageMutex.unlock();
            x["lockreleased"] = released;
            x["lockname"] = lockName;
            return crow::response(released ? 200 : 400, x);
        });

        std::thread th1(&DDserver::m_checkLifetimes);
        if (DDserver::m_is_https == true)
        {
            try
            {
                app.port(DDserver::m_port).server_name(DDserver::m_server_name).ssl_file(DDserver::m_crt_file_path, DDserver::m_key_file_path).concurrency(DDserver::m_threads).run();
            }

            catch (boost::wrapexcept<boost::system::system_error>& error)
            {
                DDserver::m_errormsg(".key / .crt file not found");
            }

            catch (...)
            {
                DDserver::m_errormsg("Unknown Error has occured");
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

    // returns a Lock if a Lock can be acquired, otherwise returns boost::none.
    boost::optional<Lock> DDserver::m_getLock(std::string lockName, const double LIFETIME){
        
        boost::optional<Lock> lock = boost::none;

        bool free {true}; // whether the lock with <lockName> is available
        DDserver::m_storageMutex.lock();
        for (long unsigned int i = 0; i < DDserver::m_lockVector.size(); i++)
            {
                if (lockName == DDserver::m_lockVector[i].m_getName() && !DDserver::m_lockVector[i].m_expired())
                {   
                    free = false;
                    break;
                }
            }
        
        // if the lock is available, create and store the lock
        if (free){
            lock = Lock(lockName, LIFETIME);
            DDserver::m_lockVector.push_back(lock.get());
        }
        DDserver::m_storageMutex.unlock();
        return lock;
    }

    // calls m_getLock until a lock has been acquired. Returns boost::none if timed-out. 
    boost::optional<Lock> DDserver::m_handleRequest(std::string lockName, const unsigned int TIMEOUT, const double LIFETIME)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
    
        boost::optional<Lock> lock = DDserver::m_getLock(lockName, LIFETIME);
        while (!lock && (std::chrono::high_resolution_clock::now() - startTime).count() < TIMEOUT)
        {  
            std::this_thread::sleep_for(std::chrono::milliseconds(DDserver::m_precision));
            lock = DDserver::m_getLock(lockName, LIFETIME);
        } 
        return lock;
    }


}
