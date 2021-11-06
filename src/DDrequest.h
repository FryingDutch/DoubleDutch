#pragma once
#include <cpr/cpr.h>
#include "crow.h"

namespace DoubleD
{
	struct DDrequest
	{
	public:
		cpr::Response response;
		std::string text;

	public:
		void GET(const char* _URL);
	};
}