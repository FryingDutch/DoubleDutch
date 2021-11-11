#include <cpr/cpr.h>
#include <string>
#include <vector>
#include "crow.h"
#include "DDserver.h"
#include "Lock.h"
#include "LockManager.h"
#include "BackupManager.h"
#include "Settings.h"


namespace DoubleD
{
	std::vector<std::string> BackupManager::backup_adresses;

	void BackupManager::sendBackup()
	{
		crow::json::wvalue x;
		std::vector<std::string> jsonList;
		x["locks"] = jsonList;

		LockManager::storageMutex.lock();
		for (size_t i = 0; i < LockManager::lockVector.size(); i++)
		{
			x["locks"][i]["lockname"] = LockManager::lockVector[i].m_getName();
			x["locks"][i]["sessiontoken"] = LockManager::lockVector[i].m_getSessionToken();
			x["locks"][i]["remaining"] = LockManager::lockVector[i].m_timeLeft();
		}
		LockManager::storageMutex.unlock();

		for (size_t i = 0; i < backup_adresses.size(); i++)
		{
			std::string url = "http://" + backup_adresses[i] + ":" + std::to_string(Settings::port) + "/backup";
			cpr::Response response = cpr::Post
			(
				cpr::Url{ url },
				cpr::Body{ x.dump() }
			);
		}
	}

	void BackupManager::receiveBackup(const crow::request& req)
	{
		std::vector<Lock> tempVec;

		crow::json::wvalue x = crow::json::load(req.body);
		for (size_t i = 0; i < x.size(); i++)
		{
			tempVec.push_back(Lock{ x["locks"][i]["lockname"].dump(), std::stod(x["locks"][i]["remaining"].dump()), x["locks"][i]["sessiontoken"].dump() });
		}
		LockManager::storageMutex.lock();
		LockManager::lockVector = tempVec;
		LockManager::storageMutex.unlock();
	}
}