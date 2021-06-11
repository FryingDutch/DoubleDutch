#define CROW_MAIN 
#include <thread>
#include "LMserver.h"

int main() 
{ 
	std::thread th1(&DoubleD::LMserver::m_startup, 8000); 
	DoubleD::LMserver::m_checkLifetimes();
	th1.join();
}
