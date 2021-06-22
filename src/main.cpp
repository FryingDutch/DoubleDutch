#include <string>
#include <csignal>
#include <iostream>
#include <thread>
#include "DDserver.h"

void errormsg(const char* message)
{
	std::cout << "[ERROR]: " << message << "! Terminating...\n";
}

bool isDigit(std::string str)
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

void handlePrefix(char prefix, int& _precision, int& _threads, bool& _is_https, int& _error, int value)
{
	switch (prefix)
	{
	case 'p':
		_precision = value;
		break;

	case 't':
		_threads = value;
		break;

	case 'h':	
		if (value == 0)
		{
			_is_https = false;
		}
		else
		{
			_error = 2;
		}
		break;

	default:
		_error = 1;
		break;
	}
}

void run(int _port, int _precision, int _threads, bool _is_https, int _error)
{
	if (_port > 0 && _precision > 0 && _threads > 0)
	{
		if (_error == 0)
		{
			DoubleD::DDserver::m_startup(_port, _threads, _precision, _is_https);
		}

		else if (_error == 1)
		{
			errormsg("Not a valid prefix");
		}

		else
		{
			errormsg("h only takes 0 as an argument");
		}
	}

	else
	{
		errormsg("Need [SIGNED INT] as argument");
	}
}

int main(int argc, char* argv[])
{
	int port;
	int precision = 333; 
	int threads = std::thread::hardware_concurrency();
	bool is_https = true;
	int error = 0;

	switch (argc)
	{
	case 2:
		if (isDigit(argv[1]))
		{
			port = std::stoi(argv[1]);

			run(port, precision, threads, is_https, error);
		}

		else
		{
			errormsg("Not a digit");
		}
		break;

	case 4:
		if (isDigit(argv[1]) && isDigit(argv[3]))
		{
			handlePrefix(*argv[2], precision, threads, is_https, error, std::stoi(argv[3]));
			port = std::stoi(argv[1]);
			run(port, precision, threads, is_https, error);
		}

		else
		{
			errormsg("Not a digit");
		}
		break;

	case 6:
		if (isDigit(argv[1]) && isDigit(argv[3]) && isDigit(argv[5]))
		{
			handlePrefix(*argv[2], precision, threads, is_https, error, std::stoi(argv[3]));
			handlePrefix(*argv[4], precision, threads, is_https, error, std::stoi(argv[5]));
			port = std::stoi(argv[1]);

			run(port, precision, threads, is_https, error);
		}
		else
		{
			errormsg("Not a digit");
		}

		break;

	case 8:
		if (isDigit(argv[1]) && isDigit(argv[3]) && isDigit(argv[5]) && isDigit(argv[7]))
		{
			handlePrefix(*argv[2], precision, threads, is_https, error, std::stoi(argv[3]));
			handlePrefix(*argv[4], precision, threads, is_https, error, std::stoi(argv[5]));
			handlePrefix(*argv[6], precision, threads, is_https, error, std::stoi(argv[7]));

			port = std::stoi(argv[1]);

			run(port, precision, threads, is_https, error);
		}

		else
		{
			errormsg("Not a digit");
		}
		break;

	default:
		errormsg("Not a valid argument count");
		break;

	}

	return 0;
}
