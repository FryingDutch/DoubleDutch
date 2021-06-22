#include <string>
#include <csignal>
#include <iostream>
#include <thread>
#include "DDserver.h"

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

void handlePrefix(char prefix, int& _precision, int& _threads, bool& _is_https, int value)
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
	{
		int https = value;
		if (https == 0)
		{
			_is_https = false;
		}
		else
		{
			_precision = 0;
		}
	}
	break;

	default:
		_precision = 0;
		break;
	}
}

void run(int _port, int _precision, int _threads, bool _is_https)
{
	if (_port > 0 && _precision > 0 && _threads > 0)
	{
		DoubleD::DDserver::m_startup(_port, _threads, _precision, _is_https);
	}

	else
	{
		std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
	}
}

int main(int argc, char* argv[])
{
	int port;
	int precision = 333; 
	int threads = std::thread::hardware_concurrency();
	bool is_https = true;

	switch (argc)
	{
	case 2:
		if (isDigit(argv[1]))
		{
			port = std::stoi(argv[1]);

			run(port, precision, threads, is_https);
		}

		else
		{
			std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
		}
		break;

	case 4:
		if (isDigit(argv[1]) && isDigit(argv[3]))
		{
			handlePrefix(*argv[2], precision, threads, is_https, std::stoi(argv[3]));
			port = std::stoi(argv[1]);
			run(port, precision, threads, is_https);
		}

		else
		{
			std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
		}
		break;

	case 6:
		if (isDigit(argv[1]) && isDigit(argv[3]) && isDigit(argv[5]))
		{
			handlePrefix(*argv[2], precision, threads, is_https, std::stoi(argv[3]));
			handlePrefix(*argv[4], precision, threads, is_https, std::stoi(argv[5]));
			port = std::stoi(argv[1]);

			run(port, precision, threads, is_https);
		}
		else
		{
			std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
		}

		break;

	case 8:
		if (isDigit(argv[1]) && isDigit(argv[3]) && isDigit(argv[5]) && isDigit(argv[7]))
		{
			handlePrefix(*argv[2], precision, threads, is_https, std::stoi(argv[3]));
			handlePrefix(*argv[4], precision, threads, is_https, std::stoi(argv[5]));
			handlePrefix(*argv[6], precision, threads, is_https, std::stoi(argv[7]));

			port = std::stoi(argv[1]);

			run(port, precision, threads, is_https);
		}
		else
		{
			std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
		}

		break;

	default:
		std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
		break;

	}

	return 0;
}
