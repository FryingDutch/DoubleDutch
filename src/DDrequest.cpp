#include <cpr/cpr.h>
#include "crow.h"
#include "DDrequest.h"

namespace DoubleD
{
	void DDrequest::GET(const char* _URL)
	{
		response = cpr::Get(cpr::Url{ _URL });
		text = response.text;
	}
}