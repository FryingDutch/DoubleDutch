#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include "LDserver.h"

namespace LyingD
{
	LDserver::LDserver(int amountOfThreads)
	{
		this->createThreads(amountOfThreads);
	}

	LDserver::~LDserver()
	{
		this->work.reset();
		this->worker_threads.join_all();
	}

	void LDserver::LOG(const char* message)
	{
		global_stream_lock.lock();
		std::cout << "[" << boost::this_thread::get_id() << "] " << message << std::endl;
		global_stream_lock.unlock();
	}

	void LDserver::printNum(int x)
	{
		std::cout << "[" << boost::this_thread::get_id() << "] Number: " << x << std::endl;
	}

	void LDserver::postPrintNum(int x)
	{
		this->strand.post(boost::bind(&LDserver::printNum, this, x));
	}

	void LDserver::workerThread()
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
					this->global_stream_lock.lock();
					std::cout << "[" << boost::this_thread::get_id() << "] " << ec << std::endl;
					this->global_stream_lock.unlock();
				}

				break;
			}

			catch (std::exception& ex)
			{
				this->global_stream_lock.lock();
				std::cout << "[" << boost::this_thread::get_id() << "] " << ex.what() << std::endl;
				this->global_stream_lock.unlock();
			}
		}
		LOG("Thread Finished");
	}

	void LDserver::createThreads(unsigned int amountOfThreads)
	{
		for (int i = 0; i < amountOfThreads; i++)
		{
			this->worker_threads.create_thread(boost::bind(&LDserver::workerThread, this));
		}
	}

	void LDserver::raiseException()
	{
		global_stream_lock.lock();
		std::cout << "[" << boost::this_thread::get_id() << "] " << __FUNCTION__ << std::endl;
		global_stream_lock.unlock();

		this->io_service->post(boost::bind(&LDserver::raiseException, this));

		throw(std::runtime_error("Srry!"));
	}
}