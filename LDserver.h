#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

class LDserver
{
public:
	boost::mutex global_stream_lock;
	void workerThread(boost::shared_ptr<boost::asio::io_service> io_service)
	{
		//LOG("Start of Thread");
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
		//LOG("Thread Finished");
	}

public:
	LDserver() = default;

	void createThreads()
	{
		this->worker_threads.create_thread(boost::bind(&LDserver::workerThread, this->io_service));
	}

	~LDserver() = default;
	boost::thread_group worker_threads;
	boost::shared_ptr<boost::asio::io_service> io_service = boost::make_shared<boost::asio::io_service>();
	boost::shared_ptr<boost::asio::io_service::work> work = boost::make_shared<boost::asio::io_service::work>(*this->io_service);
	boost::asio::io_service::strand strand = boost::asio::io_service::strand(*this->io_service);
};



