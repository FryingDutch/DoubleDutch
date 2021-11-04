#define CROW_MAIN 
#define CROW_ENABLE_SSL
#include "crow.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <optional>
#include <boost/optional.hpp>
#include "DDserver.h"

namespace DoubleD
{
    std::string DDserver::server_name = "DoubleDutch/v0.2.1";
    std::string DDserver::crt_file_path = "/certificate.crt";
    std::string DDserver::key_file_path = "/privateKey.key";
    std::string DDserver::api_key;

    int32_t DDserver::port{1};
    int32_t DDserver::precision{333};
    int32_t DDserver::threads{std::thread::hardware_concurrency()};

    bool DDserver::is_https{true}
    bool DDserver::error{false};
    bool DDserver::isRunning {true};
    bool DDserver::custom_api_key{false};

    std::vector<Lock> DDserver::lockVector;
    boost::mutex DDserver::storageMutex;

    //setting functions
    void DDserver::errormsg(const char* message)
    {
        DDserver::error = true;
        std::cerr << "[ERROR]: " << message << "! Terminating...\n";
    }

    bool DDserver::isDigit(std::string str)
    {
        for (size_t i = 0; i < str.length(); i++)
        {
            if (!std::isdigit(str[i]))
            {
                return false;
            }
        }
        return true;
    }

    void DDserver::handleCommandLineArguments(char* _argv[], int32_t _argc)
    {
        //if the argument count is higher then one (Need at least portnumber)
        //and the argument count is even (every prefix needs a value)
        //and the portnumber is an actual digit
        if (_argc > 1 && _argc % 2 == 0 && DDserver::isDigit(_argv[1]))
        {
            //set the port number
            DDserver::port = std::stoi(_argv[1]);

            enum : char
            {
                PRECISION = 'p', THREADS = 't', HTTPS = 'h', NAME = 'n', CRTFILE = 'c', KEYFILE = 'k', APIKEY = 'a'
            };

            //for every flag check and assign the user input to the correct variable
            for (size_t flag = 2; flag < _argc; flag += 2)
            {
                //if the value next to the flag is a digit
                if (DDserver::isDigit(_argv[flag + 1]))
                {
                    switch (*_argv[flag])
                    {
                    case PRECISION:
                        DDserver::precision = std::stoi(_argv[flag+1]);
                        break;

                    case THREADS:
                        DDserver::threads = std::stoi(_argv[flag+1]);
                        break;

                    case HTTPS:
                        if (std::stoi(_argv[flag+1]) == 0)
                        {
                            DDserver::is_https = false;
                        }
                        else
                        {
                            DDserver::errormsg("h only takes 0 as an argument");
                        }
                        break;

                    default:
                        DDserver::errormsg("Not a valid command-line flag");
                        break;
                    }
                }

                else
                {
                    switch (*_argv[flag])
                    {
                    case NAME:
                        DDserver::server_name = _argv[flag+1];
                        break;

                    case CRTFILE:
                        DDserver::crt_file_path = DDserver::crt_file_path.substr(0, 7) + _argv[flag+1];
                        break;

                    case KEYFILE:
                        DDserver::key_file_path = DDserver::key_file_path.substr(0, 7) + _argv[flag+1];
                        break;

                    case APIKEY:
                        DDserver::api_key = _argv[flag+1];
                        DDserver::custom_api_key = true;
                        break;

                    default:
                        DDserver::errormsg("Not a valid flag");
                        break;
                    }
                }
            }
        }

        else
        {
            DDserver::errormsg("Not a valid input");
        }
    }

    // reads the API key from a file, and assigns its string value to DDserver::api_key
    void DDserver::loadApiKey()
    {
        std::ifstream _keyFile("/config.txt", std::ios::in);
        if (_keyFile.is_open())
        {
            std::string _apiKey;
            std::getline(_keyFile, _apiKey);
            _keyFile.close();

            if (_apiKey != "") DDserver::api_key = _apiKey;

            else DDserver::errormsg("Empty key file");
        }

        else DDserver::errormsg("No API-key file found");
    }

    void DDserver::setAndBoot(int32_t _argc, char* _argv[])
    {
        DDserver::handleCommandLineArguments(_argv, _argc);
        if (!DDserver::custom_api_key) DDserver::loadApiKey();

        if (DDserver::port > 0 && DDserver::threads > 0 && DDserver::precision > -1)
        {
            if (!DDserver::error) DDserver::startup();
        }

        else
        {
            DDserver::errormsg("Need [SIGNED INT] as argument");
        }
    }

    //runtime functions
    void DDserver::startup()
    {
        crow::SimpleApp app;

        CROW_ROUTE(app, "/status")
            ([&](const crow::request& req) {

            crow::json::wvalue x;
            x["servername"] = DDserver::server_name;

            if (req.url_params.get("auth") == nullptr)
            {
                x["status"] = "no api key";
                return crow::response(400, x);
            }
            else if (DDserver::api_key != req.url_params.get("auth"))
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

            DDserver::storageMutex.lock();
            for (size_t i = 0; i < DDserver::lockVector.size(); i++) {
                x["locks"][i]["lockname"] = DDserver::lockVector[i].m_getName();
                x["locks"][i]["sessiontoken"] = DDserver::lockVector[i].m_getSessionToken();
                x["locks"][i]["remaining"] = DDserver::lockVector[i].m_timeLeft();
            }
            DDserver::storageMutex.unlock();
            return crow::response(200, x);
        });

        CROW_ROUTE(app, "/getlock")
            ([&](const crow::request& req)
        {
            crow::json::wvalue x;
            x["servername"] = DDserver::server_name;

            // lockName
            std::string _lockName;
            if (req.url_params.get("lockname") == nullptr)
            {
                x["error"] = "no lockname supplied";
                return crow::response(400, x);
            }
            _lockName = req.url_params.get("lockname");

            // auth
            if (req.url_params.get("auth") == nullptr)
            {
                x["error"] = "no api key supplied";
                return crow::response(401, x);
            }
            if (DDserver::api_key != req.url_params.get("auth"))
            {
                x["error"] = "invalid api key";
                return crow::response(401, x);
            }

            // lifetime and timeout
            double _lifetime, _timeout;
            if (req.url_params.get("lifetime") != nullptr)
            {
                _lifetime = boost::lexical_cast<double>(req.url_params.get("lifetime"));
            }
            else
            {
                _lifetime = 30.0f;
            }

            if (req.url_params.get("timeout") != nullptr)
            {
                _timeout = boost::lexical_cast<double>(req.url_params.get("timeout"));
            }
            else
            {
                _timeout = 0.0f;
            }

            boost::optional<Lock> _lock = DDserver::handleRequest(_lockName, _timeout, _lifetime);
            x["sessiontoken"] = _lock ? _lock.get().m_getSessionToken() : "";
            x["lockacquired"] = _lock ? true : false;
            x["lockname"] = _lockName;
            return crow::response(200, x);
        });

        // Releasing the lock
        CROW_ROUTE(app, "/releaselock").methods("DELETE"_method)
            ([&](const crow::request& req) {
            std::string _lockName, _session_token;
            crow::json::wvalue x;
            x["servername"] = DDserver::server_name;

            if (req.url_params.get("lockname") == nullptr || req.url_params.get("token") == nullptr)
            {
                x["error"] = "invalid params";
                return crow::response(400, x);
            }

            else
            {
                _lockName = req.url_params.get("lockname");
                _session_token = req.url_params.get("token");
            }

            bool _released{ false };
            DDserver::storageMutex.lock();
            for (long unsigned int i = 0; i < DDserver::lockVector.size(); i++) {

                if (_lockName == DDserver::lockVector[i].m_getName() &&
                    _session_token == DDserver::lockVector[i].m_getSessionToken())
                {
                    DDserver::lockVector.erase(DDserver::lockVector.begin() + i);
                    DDserver::lockVector.shrink_to_fit();
                    _released = true;
                    break;
                }
            }
            DDserver::storageMutex.unlock();
            x["lockreleased"] = _released;
            x["lockname"] = _lockName;
            return crow::response(_released ? 200 : 400, x);
        });
        std::thread _lifeTime_thread(&DDserver::checkLifetimes);        

        // configure the app instance with given parameters
        app.port(DDserver::port).server_name(DDserver::server_name).concurrency(DDserver::threads);

        if (DDserver::is_https) app.ssl_file(DDserver::crt_file_path, DDserver::key_file_path);

        try
        {
            app.run();
        }

        catch (boost::wrapexcept<boost::system::system_error>& error)
        {
            DDserver::errormsg(".key / .crt file not found");
        }

        catch (const std::exception& ex)
        {
            DDserver::errormsg("An error has occurred. ");
            std::cerr << ex.what() << std::endl;
        }
        DDserver::isRunning = false;
        _lifeTime_thread.join();
    }

    void DDserver::checkLifetimes()
    {
        while (DDserver::isRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(DDserver::precision));
            DDserver::storageMutex.lock();
            for (unsigned int i = 0; i < DDserver::lockVector.size(); i++)
            {
                if (DDserver::lockVector[i].m_expired())
                {
                    DDserver::lockVector.erase(DDserver::lockVector.begin() + i);
                    DDserver::lockVector.shrink_to_fit();
                }
            }
            DDserver::storageMutex.unlock();
        }
    }

    // returns a Lock if a Lock can be acquired, otherwise returns boost::none.
    boost::optional<Lock> DDserver::getLock(std::string lockName, const double LIFETIME) {

        // determine whether the lock with <lockName> is free/available
        bool _free{ true };
        DDserver::storageMutex.lock();
        for (size_t i = 0; i < DDserver::lockVector.size(); i++)
        {
            if (lockName == DDserver::lockVector[i].m_getName() && !DDserver::lockVector[i].m_expired())
            {
                _free = false;
                break;
            }
        }

        // insert a Lock if <lockName> is free/available
        boost::optional<Lock> _lock;
        if (_free) {
            _lock = Lock(lockName, LIFETIME);
            DDserver::lockVector.push_back(_lock.get());
        }
        DDserver::storageMutex.unlock();
        return _lock;
    }

    // calls m_getLock until a lock has been acquired. Returns boost::none if timed-out. 
    boost::optional<Lock> DDserver::handleRequest(std::string lockName, const uint32_t TIMEOUT, const double LIFETIME)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> difference;
        for (;;)
        {
            boost::optional<Lock> lock = DDserver::getLock(lockName, LIFETIME);
            auto currentTime = std::chrono::high_resolution_clock::now();
            difference = currentTime - startTime;

            if (lock || difference.count() > TIMEOUT) return lock;

            std::this_thread::sleep_for(std::chrono::milliseconds(DDserver::precision));
        }
    }
}
