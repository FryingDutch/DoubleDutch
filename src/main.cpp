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

int main(int argc, char* argv[])
{
	int port;
	int precision = 333; 
	int threads = std::thread::hardware_concurrency();
	switch (argc)
	{
	case 2:
		if (isDigit(argv[1]))
		{
			port = std::stoi(argv[1]);

			if (port > 0 && threads > 0 && precision > 0)
			{
				DoubleD::DDserver::m_startup(port, threads, precision);
			}

			else
			{
				std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
			}
		}

		else
		{
			std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
		}
		break;

	case 4:
		if (isDigit(argv[1]) && isDigit(argv[3]))
		{
			switch (*argv[2])
			{
			case 'p':
				precision = std::stoi(argv[3]);
				break;

			case 't':
				threads = std::stoi(argv[3]);
				break;

			default:
				precision = 0;
				break;
			}

			port = std::stoi(argv[1]);

			if (port > 0 && threads > 0 && precision > 0)
			{
				DoubleD::DDserver::m_startup(port, threads, precision);
			}

			else
			{
				std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
			}
		}

		else
		{
			std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
		}
		break;

	case 6:
		if (isDigit(argv[1]) && isDigit(argv[3]) && isDigit(argv[5]))
		{
			switch (*argv[2])
			{
			case 'p':
				precision = std::stoi(argv[3]);
				break;

			case 't':
				threads = std::stoi(argv[3]);
				break;

			default:
				precision = 0;
				break;
			}

			switch (*argv[4])
			{
			case 'p':
				precision = std::stoi(argv[5]);
				break;

			case 't':
				threads = std::stoi(argv[5]);
				break;

			default:
				precision = 0;
				break;
			}
			port = std::stoi(argv[1]);

			if (port > 0 && threads > 0 && precision > 0)
			{
				DoubleD::DDserver::m_startup(port, threads, precision);
			}

			else
			{
				std::cout << "[ERROR]: Invalid arguments! Terminating...\n";
			}
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
