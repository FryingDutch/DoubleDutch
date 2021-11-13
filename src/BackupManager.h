#pragma once
#include "crow.h"

namespace DoubleD
{
	struct BackupManager
	{
		struct BackupAdres
		{
			inline static std::string adres;
			inline static uint32_t tries;
		};

		static std::vector<BackupAdres> backup_adresses;

		static uint32_t sendBackup();
		static void receiveBackup(const crow::request& req);
	};
}