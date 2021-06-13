#define CROW_MAIN 
#include <thread>
#include "DDserver.h"

int main() 
{ 
	std::thread th1(&DoubleD::DDserver::m_checkLifetimes);
	DoubleD::DDserver::m_startup(8000);	
	th1.join();
}
