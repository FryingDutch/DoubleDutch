#pragma once
#include <cpr/cpr.h>
#include "crow.h"

namespace DoubleD
{
	struct Request
	{
	public:
		cpr::Response response;
		std::string text;

	public: 
		void GET(const char* _URL);
		void POST(const char* _URL, std::string _apikey, std::string _lockname);
	};
}