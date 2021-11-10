#include <string>
#include "BackupManager.h"
#include "Request.h"
#include "Settings.h"

namespace DoubleD
{
	void BackupManager::sendBackup(std::string _ip, std::string _lockName)
	{
		Request r;
		std::string url = "http://" + _ip + ":" + std::to_string(Settings::port) + "/getlock?auth=" + Settings::api_key + "&lockname=" + _lockName;
		r.GET(url.c_str());
	}

	void BackupManager::receiveBackup()
	{

	}
}