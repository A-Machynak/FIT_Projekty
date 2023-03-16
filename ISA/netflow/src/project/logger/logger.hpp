/**
 * @file logger.hpp
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Implementace
 */

#include "logger.h"

namespace Logger
{
	template <typename T>
	void Log(T msg, LogLevel level)
	{
		switch (level) {
		case LogLevel::Info:
			LogInfo(msg);
			break;
		case LogLevel::Warning:
			LogInfo(msg);
			break;
		case LogLevel::Error:
			LogInfo(msg);
			break;
		case LogLevel::Debug:
			LogInfo(msg);
			break;
		default:
			std::cerr << "[Unknown log level]: " << msg << "\n";
			break;
		}
	}

	template <typename T>
	void LogInfo(T msg)
	{
		std::cout << "[+] INFO: " << msg << "\n";
	}

	template <typename T>
	void LogWarning(T msg)
	{
		std::cerr << "[!] WARNING: " << msg << "\n";
	}

	template <typename T>
	void LogError(T msg)
	{
		std::cerr << "[-] ERROR: " << msg << "\n";
	}

	template <typename T>
	void LogDebug(T msg)
	{
		#ifdef _loggerDebug
		std::cout << "[?] DEBUG: " << msg << "\n";
		#endif
	}
} // namespace Logger
