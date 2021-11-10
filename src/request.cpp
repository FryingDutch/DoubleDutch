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
		status_code = response.status_code;

		if (status_code == 200)
		{
			try
			{
				json = crow::json::load(response.text);
			}

			catch (...)
			{
				std::cerr << "[Request]: Not a valid JSON return!\n";
			}
		}
		return *this;
	}

	void Request::POST(const char* _URL, std::string _apikey, std::string _lockname)
	{
		cpr::Response response = cpr::Post(cpr::Url{ _URL },
			cpr::Payload{ {"auth", _apikey},{"lockname", _lockname } }
		);
	}

	crow::json::wvalue& Request::JSON()
	{
		return json;
	}
	
	std::string& Request::string()
	{
		return text;
	}

	uint32_t& Request::statusCode()
	{
		return status_code;
	}
}