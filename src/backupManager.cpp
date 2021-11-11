#include <cpr/cpr.h>
#include <string>
#include "crow.h"
#include "DDserver.h"
#include "LockManager.h"
#include "BackupManager.h"
#include "Request.h"
#include "Settings.h"

namespace DoubleD
{
	void BackupManager::sendBackup(std::string _ip)
	{
		/*
		Request r;
		std::string url = "http://" + _ip + ":" + std::to_string(Settings::port) + "/getlock?auth=" + Settings::api_key + "&lockname=" + _lockName;
		r.GET(url.c_str());
		*/

		crow::json::wvalue x;
		std::vector<std::string> jsonList;
		x["locks"] = jsonList;

		DDserver::storageMutex.lock();
		for (size_t i = 0; i < LockManager::lockVector.size(); i++)
		{
			x["locks"][i]["lockname"] = LockManager::lockVector[i].m_getName();
			x["locks"][i]["sessiontoken"] = LockManager::lockVector[i].m_getSessionToken();
			x["locks"][i]["remaining"] = LockManager::lockVector[i].m_timeLeft();
		}
		DDserver::storageMutex.unlock();

		std::string url = "http://" + _ip + ":" + std::to_string(Settings::port) + "/status";
		cpr::Response response = cpr::Post
		(
			cpr::Url{ url },
			cpr::Body{ {"auth", "randomapikey"} }
		);
	}

	void BackupManager::receiveBackup()
	{

	}
}