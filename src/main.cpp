#define CROW_MAIN 
#include <thread>
#include <string>
#include <iostream>
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
	if (argc > 1)
	{
		if (isDigit(argv[1]))
		{
			int port = std::stoi(argv[1]);
			std::thread th1(&DoubleD::DDserver::m_startup, port);
			DoubleD::DDserver::m_checkLifetimes();
			th1.join();
		}

		else { std::cout << "ERROR: Not a digit. Terminating...\n"; }
	}

	else { std::cout << "ERROR: No portnumber has been given. Terminating...\n"; }

	return 0;
}
