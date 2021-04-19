#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "LDserver.h"

LyingD::LDserver s1(2);

int main(int argc, char* argv[])
{	
	s1.mainThread();
	//s1.io_service->post(boost::bind(&LDserver::raiseException, &s1));

	return 0;
}
