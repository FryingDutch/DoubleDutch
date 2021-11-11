#pragma once
#include <iostream>
#include <string>
#include <cpr/cpr.h>
#include "crow.h"

namespace DoubleD
{
	struct Request
	{
	private:
		uint32_t status_code;
		std::string text;
		crow::json::wvalue json;

	public: 
		Request& GET(const char* _URL);
		Request& POST(const char* _URL);
		crow::json::wvalue& JSON();
		std::string& string();
		uint32_t& statusCode();
	};
}