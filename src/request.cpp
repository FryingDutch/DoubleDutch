#include <cpr/cpr.h>
#include <string>
#include "crow.h"
#include "Request.h"

namespace DoubleD
{
	Request& Request::GET(const char* _URL)
	{
		cpr::Response response = cpr::Get(cpr::Url{ _URL });
		text = response.text;

		auto x = crow::json::load(response.text);
		json = x;
		
		return *this;
	}

	crow::json::wvalue Request::JSON()
	{
		return json;
	}

	void Request::POST(const char* _URL, std::string _apikey, std::string _lockname)
	{
		cpr::Response response = cpr::Post(cpr::Url{ _URL },
			cpr::Payload{ {"auth", _apikey},{"lockname", _lockname }}
		);
	}
}