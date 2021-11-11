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
		if (backup_adresses.size() > 0)
		{
			// Take the same algorithm as used for the status endpoint to create the json
			crow::json::wvalue x;
			std::vector<std::string> jsonList;
			x["locks"] = jsonList;
			x["auth"] = Settings::api_key;

			LockManager::storageMutex.lock();
			for (size_t i = 0; i < LockManager::lockVector.size(); i++)
			{
				x["locks"][i]["lockname"] = LockManager::lockVector[i].m_getName();
				x["locks"][i]["sessiontoken"] = LockManager::lockVector[i].m_getSessionToken();
				x["locks"][i]["remaining"] = LockManager::lockVector[i].m_timeLeft();
			}
			LockManager::storageMutex.unlock();

			// loop trough all the IP adresses of the backupserver
			for (size_t i = 0; i < backup_adresses.size(); i++)
			{
				// check if request needs to be http(s)
				std::string url{ Settings::is_https ? "https://" : "http://" };
				url += backup_adresses[i] + ":" + std::to_string(Settings::port) + "/backup";

				// send POST request with all the locks
				cpr::Response response = cpr::Post
				(
					cpr::Url{ url },
					cpr::Body{ x.dump() }
				);
			}
		}
	}

	void BackupManager::receiveBackup(const crow::request& req)
	{
		// create temp empty vector
		std::vector<Lock> tempVec;

		// get the JSON from the POST request, containing all the locks + auth;
		crow::json::wvalue x = crow::json::load(req.body);

		// check if the api-key is the same for the main and backupserver.
		if (x["auth"].dump() == Settings::api_key)
		{
			// loop trough all the locks and add them to the empty vector
			for (size_t i = 0; i < x["locks"].size(); i++)
			{
				tempVec.push_back(Lock{ x["locks"][i]["lockname"].dump(), std::stod(x["locks"][i]["remaining"].dump()), x["locks"][i]["sessiontoken"].dump() });
			}

			// Change the lockvector to be a copy of the tempvec
			LockManager::storageMutex.lock();
			LockManager::lockVector = tempVec;
			LockManager::storageMutex.unlock();
		}
	}
}