#pragma once
#include <cpr/cpr.h>
#include "crow.h"

namespace DoubleD
{
	struct Request
	{
	public:
		std::string text;
		crow::json::wvalue json;

	public: 
		Request& GET(const char* _URL);
		void POST(const char* _URL, std::string _apikey, std::string _lockname);
		crow::json::wvalue JSON();
	};
}