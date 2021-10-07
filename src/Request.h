#pragma once
#include <curl/curl.h>
#include <iostream>
#include "crow.h"
#include "DDserver.h"

struct Request
{
public:
    uint32_t statusCode = 0;
    std::string data;

private:
	static size_t curlCallBack(void* contents, size_t size, size_t nmemb, std::string* str)
	{
		size_t newLength = size * nmemb;
		try
		{
			str->append((char*)contents, newLength);
		}
		catch (std::bad_alloc& exception)
		{
			return 0;
		}
		return newLength;
	}

public:
    void get(const char* _URL)
	{
        statusCode = 0;
        data = "";
        CURL* session;
        
        curl_global_init(CURL_GLOBAL_DEFAULT);

        session = curl_easy_init();

        if (session)
        {
            curl_easy_setopt(session, CURLOPT_URL, _URL);

            // for HTTPS
            
            curl_easy_setopt(session, CURLOPT_SSL_VERIFYPEER, DoubleD::DDserver::is_https ? 1 : 0);
            curl_easy_setopt(session, CURLOPT_SSL_VERIFYHOST, DoubleD::DDserver::is_https ? 1 : 0);

            curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, curlCallBack);
            curl_easy_setopt(session, CURLOPT_WRITEDATA, &data);

            CURLcode response{ curl_easy_perform(session) };
           
            if (response != CURLE_OK)
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(response));
                return;
            }

            curl_easy_getinfo(session, CURLINFO_RESPONSE_CODE, &statusCode);
            curl_easy_cleanup(session);
        }
	}
};