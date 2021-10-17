#pragma once
#include <curl/curl.h>
#include <iostream>
#include "DDserver.h"

struct Request
{
private:
    bool m_httpsRequest;

    uint32_t m_statusCode{ 0 };
    std::string m_data;

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
    Request(bool useHTTPS = false) :
        m_httpsRequest(useHTTPS) {};

    uint32_t getStatusCode() { return m_statusCode; }
    std::string getData() 
    { 
        return m_data; 
    }

    void postStatus(std::string _URL, const char* _payload)
    {
        CURL* session;

        curl_global_init(CURL_GLOBAL_DEFAULT);
        session = curl_easy_init();

        if (session)
        {
            // Reset members and args for every request made by the object
            m_statusCode = 0;
            m_data = "";

            curl_easy_setopt(session, CURLOPT_URL, _URL.c_str());

            // for HTTPS
            curl_easy_setopt(session, CURLOPT_SSL_VERIFYPEER, m_httpsRequest ? 1L : 0L);
            curl_easy_setopt(session, CURLOPT_SSL_VERIFYHOST, m_httpsRequest ? 1L : 0L);

            // Wrting the response m_data of the URL to the m_data member
            curl_easy_setopt(session, CURLOPT_WRITEFUNCTION, curlCallBack);
            curl_easy_setopt(session, CURLOPT_WRITEDATA, &m_data);

            // Execute the transfer
            CURLcode exit_code{ curl_easy_perform(session) };

            if (exit_code != CURLE_OK)
            {
                // use fprintf instead of std::cerr to keep the function in C as much as possible
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(exit_code));
            }

            curl_easy_getinfo(session, CURLINFO_RESPONSE_CODE, &exit_code);
            curl_easy_cleanup(session);
        }
    }
};