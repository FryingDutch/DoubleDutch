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
		this->runTimer(5);
	}

	LDserver::~LDserver()
	{
		this->work.reset();
		this->worker_threads.join_all();
		//this->pool.join();
	}

	void LDserver::timerHandler(const bsys::error_code& error)
	{
		if (error)
		{
			//foo
		}
		else
		{
			std::cout << "Timepassing" << std::endl;
			//bar
		}
	}

	void LDserver::LOG(const char* message)
	{
		global_stream_lock.lock();
		std::cout << "[" << boost::this_thread::get_id() << "] " << message << std::endl;
		global_stream_lock.unlock();
	}

	void LDserver::runTimer(int seconds)
	{
		this->timer.expires_from_now(boost::posix_time::seconds(seconds));
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
				this->timer.async_wait(boost::bind(&LDserver::timerHandler, this, ec));
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

	void LDserver::mainThread()
	{
		this->global_stream_lock.lock();
		std::cout << "[" << boost::this_thread::get_id() << "] This wil exit when all work is finished" << std::endl;
		this->global_stream_lock.unlock();

		this->postPrintNum(1);
		this->postPrintNum(2);
		this->postPrintNum(3);
		this->postPrintNum(4);
	}
}
