#pragma once
#include <iostream>

namespace DoubleD
{
	struct Settings
	{
	private:
		static std::string server_name, crt_file_path, key_file_path, api_key;
		static int32_t port, precision, threads;
		static bool is_https, error, isRunning, custom_api_key, hasBackup, needBackup;

	private:
		static void errormsg(const char* message);
		static bool isDigit(std::string str);
		static void handleCommandLineArguments(char* _argv[], uint32_t _argc);
		static void loadApiKey();

		friend class DDserver;
		friend class LockManager;
		friend class BackupManager;
	};
}