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
	if (argc > 1 && argc < 4)
	{
		if (isDigit(argv[1]))
		{
			int port = std::stoi(argv[1]);
			if (port > 0)
			{
				int numOfThreads;
				if (argc == 2)
				{
					numOfThreads = std::thread::hardware_concurrency();
				}
				else if(isDigit(argv[2]))
				{
					numOfThreads = std::stoi(argv[2]);
				}

				else 
				{ 
					std::cout << "[ERROR]: Not a digit[THREADS]. Terminating...\n"; return 0; 
				}
				
				if(numOfThreads < 1) 
				{ 
					std::cout << "[ERROR]: Need at least one thread! Terminating...\n" ; return 0; 
				}

				std::thread th1(&DoubleD::DDserver::m_startup, port, numOfThreads);
				DoubleD::DDserver::m_checkLifetimes();
				th1.join();
			}
			else { std::cout << "[ERROR]: Not a valid[UNSIGNED INT] digit[PORTNUM]. Terminating...\n"; }
		}
			else { std::cout << "[ERROR]: Not a digit[PORTNUM]. Terminating...\n"; }
	}
	else { std::cout << "[ERROR]: No valid argument([PORTNUM] ?[THREADS]) has been given. Terminating...\n"; }

	return 0;
}
