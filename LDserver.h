#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace LyingD
{
	class LDserver
	{
	private:
		boost::thread_group worker_threads;
		std::shared_ptr<boost::asio::io_service> io_service = std::make_shared<boost::asio::io_service>();
		std::shared_ptr<boost::asio::io_service::work> work = std::make_shared<boost::asio::io_service::work>(*this->io_service);
		boost::asio::io_service::strand strand = boost::asio::io_service::strand(*this->io_service);

	private:
		void LOG(const char* message);
		void printNum(int x);
		void workerThread();
		void createThreads(unsigned int amountOfThreads);
		void raiseException();

	public:
		boost::mutex global_stream_lock;

	public:
		LDserver(int amountOfThreads);
		~LDserver();

		void postPrintNum(int x);
	};
}


