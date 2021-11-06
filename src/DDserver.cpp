#define CROW_MAIN 
#define CROW_ENABLE_SSL
#include "crow.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <optional>
#include "DDserver.h"
#include "Settings.h"

namespace DoubleD
{
    std::vector<Lock> DDserver::lockVector;
    std::mutex DDserver::storageMutex;

    //runtime functions
    void DDserver::startup()
    {
        crow::SimpleApp app;

        CROW_ROUTE(app, "/status")
            ([&](const crow::request& req) {

            crow::json::wvalue x;
            x["servername"] = Settings::server_name;

            if (req.url_params.get("auth") == nullptr)
            {
                x["status"] = "no api key";
                return crow::response(400, x);
            }
            else if (Settings::api_key != req.url_params.get("auth"))
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
            x["servername"] = Settings::server_name;

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
            if (Settings::api_key != req.url_params.get("auth"))
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

            std::optional<Lock> _lock = DDserver::handleRequest(_lockName, _timeout, _lifetime);
            x["sessiontoken"] = _lock ? _lock.value().m_getSessionToken() : "";
            x["lockacquired"] = _lock ? true : false;
            x["lockname"] = _lockName;
            return crow::response(200, x);
        });

        // Releasing the lock
        CROW_ROUTE(app, "/releaselock").methods("DELETE"_method)
            ([&](const crow::request& req) {
            std::string _lockName, _session_token;
            crow::json::wvalue x;
            x["servername"] = Settings::server_name;

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
            for (size_t i = 0; i < DDserver::lockVector.size(); i++) {

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
        app.port(Settings::port).server_name(Settings::server_name).concurrency(Settings::threads);

        if (Settings::is_https) app.ssl_file(Settings::crt_file_path, Settings::key_file_path);

        try
        {
            app.run();
        }

        catch (boost::wrapexcept<boost::system::system_error>& error)
        {
            Settings::errormsg(".key / .crt file not found");
        }

        catch (const std::exception& ex)
        {
            Settings::errormsg("An error has occurred. ");
            std::cerr << ex.what() << std::endl;
        }
        Settings::isRunning = false;
        _lifeTime_thread.join();
    }

    void DDserver::checkLifetimes()
    {
        while (Settings::isRunning)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(Settings::precision));
            DDserver::storageMutex.lock();
            for (size_t i = 0; i < DDserver::lockVector.size(); i++)
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
    std::optional<Lock> DDserver::getLock(std::string lockName, const double LIFETIME) {

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
        std::optional<Lock> _lock;
        if (_free) {
            _lock = Lock(lockName, LIFETIME);
            DDserver::lockVector.push_back(_lock.value());
        }
        DDserver::storageMutex.unlock();
        return _lock;
    }

    // calls m_getLock until a lock has been acquired. Returns boost::none if timed-out. 
    std::optional<Lock> DDserver::handleRequest(std::string lockName, const uint32_t TIMEOUT, const double LIFETIME)
    {
        auto startTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> difference;
        for (;;)
        {
            std::optional<Lock> lock = DDserver::getLock(lockName, LIFETIME);
            auto currentTime = std::chrono::high_resolution_clock::now();
            difference = currentTime - startTime;

            if (lock || difference.count() > TIMEOUT) return lock;

            std::this_thread::sleep_for(std::chrono::milliseconds(Settings::precision));
        }
    }

    void DDserver::setAndBoot(int32_t _argc, char* _argv[])
    {
        Settings::handleCommandLineArguments(_argv, _argc);
        if (!Settings::custom_api_key) Settings::loadApiKey();

        if (Settings::port > 0 && Settings::threads > 0 && Settings::precision > -1)
        {
            if (!Settings::error) 
                DDserver::startup();
        }

        else
        {
            Settings::errormsg("Need [SIGNED INT] as argument");
        }
    }
}
