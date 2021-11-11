#pragma once
#include "crow.h"

namespace DoubleD
{
	struct BackupManager
	{
		static std::vector<std::string> backup_adresses;

		static void sendBackup();
		static void receiveBackup(const crow::request& req);
	};
}