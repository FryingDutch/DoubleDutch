#pragma once

namespace DoubleD
{
	struct BackupManager
	{
		static void sendBackup(std::string _ip, std::string _lockName);
		static void receiveBackup();
	};
}