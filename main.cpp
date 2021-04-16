#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "LDserver.h"

int main(int argc, char* argv[])
{
	LyingD::LDserver s1(2);

	s1.global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id() << "] This wil exit when all work is finished" << std::endl;	
	s1.global_stream_lock.unlock();

	s1.postPrintNum(1);
	s1.postPrintNum(2);
	s1.postPrintNum(3);
	s1.postPrintNum(4);

//	s1.io_service->post(boost::bind(&LDserver::raiseException, &s1));

	return 0;
}
