#define CROW_MAIN 
#include <thread>
#include <string>
#include <iostream>
#include "DDserver.h"

bool isString(std::string str)
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

int main() 
{ 
	std::string input;

	do {
		std::cout << "Choose a portnumber: ";
		std::getline(std::cin, input);
	} while (!isString(input));

	int port = std::stoi(input);

	std::thread th1(&DoubleD::DDserver::m_startup, port); 
	DoubleD::DDserver::m_checkLifetimes();
	th1.join();
}
