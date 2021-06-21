#include <string>
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

//Need to reduce the amount of code
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
		}
		break;

	case 3:
		if (isDigit(argv[1]) && isDigit(argv[2]))
		{
			port = std::stoi(argv[1]);
			threads = std::stoi(argv[2]);

			if (port > 0 && threads > 0 && precision > 0)
			{
				DoubleD::DDserver::m_startup(port, threads, precision);
			}
		}		
		break;

	case 4:
		if (isDigit(argv[1]) && *argv[2] == 'p' && isDigit(argv[3]))
		{
			port = std::stoi(argv[1]);
			precision = std::stoi(argv[3]);

			if (port > 0 && threads > 0 && precision > 0)
			{
				DoubleD::DDserver::m_startup(port, threads, precision);
			}
		}
		break;

	case 5:
		if (isDigit(argv[1]) && *argv[2] == 'p' && isDigit(argv[3]) && isDigit(argv[4]))
		{
			port = std::stoi(argv[1]);
			precision = std::stoi(argv[3]);
			threads = std::stoi(argv[4]);

			if (port > 0 && threads > 0 && precision > 0)
			{
				DoubleD::DDserver::m_startup(port, threads, precision);
			}
		}

		else if (isDigit(argv[1]) && isDigit(argv[2]) && *argv[3] == 'p' && isDigit(argv[4]))
		{
			port = std::stoi(argv[1]);
			precision = std::stoi(argv[4]);
			threads = std::stoi(argv[2]);

			if (port > 0 && threads > 0 && precision > 0)
			{
				DoubleD::DDserver::m_startup(port, threads, precision);
			}
		}
		break;

	default:
		break;

	}

	return 0;
}
