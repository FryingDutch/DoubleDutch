#include <cpr/cpr.h>
#include <string>
#include "crow.h"
#include "Request.h"

namespace DoubleD
{
	void Request::GET(const char* _URL)
	{
		response = cpr::Get(cpr::Url{ _URL });
		text = response.text;
	}

	void Request::POST(const char* _URL, std::string _apikey, std::string _lockname)
	{
		cpr::Response r = cpr::Post(cpr::Url{ _URL },
			cpr::Payload{ {"auth", _apikey},{"lockname", _lockname }}
		);

		text = response.text;
	}
}