#pragma once

namespace DoubleD
{
	struct BackupManager
	{
		static void sendBackup(std::string _ip);
		static void receiveBackup();
	};
}