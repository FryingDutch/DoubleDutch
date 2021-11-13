#include <thread>
#include <fstream>
#include "Settings.h"

namespace DoubleD
{
    std::string Settings::server_name = "DoubleDutch/v0.3.1";
    std::string Settings::crt_file_path = "/certificate.crt";
    std::string Settings::key_file_path = "/privateKey.key";
    std::string Settings::api_key;

    int32_t Settings::port{ 1 };
    int32_t Settings::precision{ 333 };
    int32_t Settings::threads{ (int32_t)std::thread::hardware_concurrency() };

    bool Settings::is_https{ true };
    bool Settings::error{ false };
    bool Settings::isRunning{ true };
    bool Settings::custom_api_key{ false };
    bool Settings::hasBackup{ false };
    bool Settings::needBackup{ false };


    //setting functions
    void Settings::errormsg(const char* message)
    {
        error = true;
        std::cerr << "[ERROR]: " << message << "! Terminating...\n";
    }

    bool Settings::isDigit(std::string str)
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

    void Settings::handleCommandLineArguments(char* _argv[], uint32_t _argc)
    {
        //if the argument count is higher then one (Need at least portnumber)
        //and the argument count is even (every prefix needs a value)
        //and the portnumber is an actual digit
        if (_argc > 1 && _argc % 2 == 0 && Settings::isDigit(_argv[1]))
        {
            //set the port number
            port = std::stoi(_argv[1]);

            enum : char
            {
                PRECISION = 'p', THREADS = 't', HTTPS = 'h', NAME = 'n', CRTFILE = 'c', KEYFILE = 'k', APIKEY = 'a'
            };

            //for every flag check and assign the user input to the correct variable
            for (size_t flag = 2; flag < _argc; flag += 2)
            {
                //if the value next to the flag is a digit
                if (isDigit(_argv[flag + 1]))
                {
                    switch (*_argv[flag])
                    {
                    case PRECISION:
                        precision = std::stoi(_argv[flag + 1]);
                        break;

                    case THREADS:
                        threads = std::stoi(_argv[flag + 1]);
                        break;

                    case HTTPS:
                        if (std::stoi(_argv[flag + 1]) == 0)
                        {
                            is_https = false;
                        }
                        else
                        {
                            errormsg("h only takes 0 as an argument");
                        }
                        break;

                    default:
                        errormsg("Not a valid command-line flag");
                        break;
                    }
                }

                else
                {
                    switch (*_argv[flag])
                    {
                    case NAME:
                        server_name = _argv[flag + 1];
                        break;

                    case APIKEY:
                        api_key = _argv[flag + 1];
                        custom_api_key = true;
                        break;

                    default:
                        errormsg("Not a valid flag");
                        break;
                    }
                }
            }
        }

        else
        {
            errormsg("Not a valid input");
        }
    }

    // reads the API key from a file, and assigns its string value to Settings::api_key
    void Settings::loadApiKey()
    {
        std::ifstream _keyFile("/config.txt", std::ios::in);
        if (_keyFile.is_open())
        {
            std::string _apiKey;
            std::getline(_keyFile, _apiKey);
            _keyFile.close();

            if (_apiKey != "") 
                api_key = _apiKey;

            else 
                errormsg("Empty key file");
        }

        else errormsg("No API-key file found");
    }
}