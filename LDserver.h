#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

namespace LyingD
{
	namespace net = boost::asio;
	namespace bsys = boost::system;

	class LDserver
	{
	private:
		//using executor_type = net::thread_pool::executor_type;

		/*net::thread_pool pool{ 2 };
		net::strand<executor_type> strand{ pool.get_executor() };*/
		boost::thread_group worker_threads;
		std::shared_ptr<net::io_service> io_service = std::make_shared<net::io_service>();
		std::shared_ptr<net::io_service::work> work = std::make_shared<net::io_service::work>(*this->io_service);
		net::io_service::strand strand = net::io_service::strand(*this->io_service);
		net::deadline_timer timer = net::deadline_timer(*this->io_service);

	private:
		void LOG(const char* message);
		void runTimer(int seconds);
		void printNum(int x);
		void timerHandler(const bsys::error_code& error);
		void workerThread();
		void createThreads(unsigned int amountOfThreads);
		void raiseException();

	public:
		boost::mutex global_stream_lock;

	public:
		LDserver(int amountOfThreads);
		~LDserver();

		void mainThread();
		void postPrintNum(int x);
	};
}


