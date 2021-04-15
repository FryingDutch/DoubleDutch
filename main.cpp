#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "LDserver.h"

boost::mutex global_stream_lock;

void LOG(const char* message)
{
	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id() << "] " << message << std::endl;
	global_stream_lock.unlock();
}

void raiseException(boost::shared_ptr<boost::asio::io_service> io_service)
{
	global_stream_lock.lock();
	std::cout << "[" << boost::this_thread::get_id() << "] " << __FUNCTION__ << std::endl;
	global_stream_lock.unlock();

	io_service->post(boost::bind(&raiseException, io_service));

	throw(std::runtime_error("Srry!"));
}

void workerThread(boost::shared_ptr<boost::asio::io_service> io_service)
{
	LOG("Start of Thread");
	for (;;)
	{
		try
		{
			boost::system::error_code ec;
			io_service->run(ec);
			if (ec)
			{
				global_stream_lock.lock();
				std::cout << "[" << boost::this_thread::get_id() << "] " << ec << std::endl;
				global_stream_lock.unlock();
			}

			break;
		}

		catch(std::exception &ex)
		{
			global_stream_lock.lock();
			std::cout << "[" << boost::this_thread::get_id() << "] " << ex.what() << std::endl;
			global_stream_lock.unlock();
		}
	}

	LOG("Thread Finished");
}

void printNum(int x)
{
	std::cout << "[" << boost::this_thread::get_id() << "] Number: " << x << std::endl;
}

int main(int argc, char* argv[])
{
	LDserver s1;

	std::cout << "[" << boost::this_thread::get_id() << "] This wil exit when all work is finished" << std::endl;

	
	for (int i = 0; i <= 2; i++)
	{
		s1.worker_threads.create_thread(boost::bind(&workerThread, s1.io_service));
	}

	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
	
	s1.strand.post(boost::bind(&printNum, 1));
	s1.strand.post(boost::bind(&printNum, 2));
	s1.strand.post(boost::bind(&printNum, 3));
	s1.strand.post(boost::bind(&printNum, 4));
	s1.strand.post(boost::bind(&printNum, 5));
	
	//s1.io_service->post(boost::bind(&raiseException, s1.io_service));

	s1.work.reset();
	s1.worker_threads.join_all();

	return 0;
}