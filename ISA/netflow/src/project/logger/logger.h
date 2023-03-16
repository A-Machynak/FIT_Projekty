/**
 * @file logger.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Jednoduché funkce pro logování
 */

#pragma once

#include <string>
#include <iostream>

namespace Logger
{
	/**
	 * @brief Typy logovacích zpráv
	 */
	enum class LogLevel
	{
		Info,
		Warning,
		Error,
		Debug
	};

	/**
	 * @brief Vypíše zprávu na stderr nebo stdout (dle typu zprávy)
	 * 
	 * @param msg zpráva
	 * @param level typ zprávy
	 */
	template <typename T>
	void Log(T msg, LogLevel level = LogLevel::Debug);

	/**
	 * @brief Vypíše zprávu úrovně "Info" na stdout
	 * 
	 * @param value zpráva
	 */
	template <typename T>
	void LogInfo(T msg);

	/**
	 * @brief Vypíše zprávu úrovně "Warning" na stderr
	 * 
	 * @param value zpráva
	 */
	template <typename T>
	void LogWarning(T msg);

	/**
	 * @brief Vypíše zprávu úrovně "Error" na stderr
	 * 
	 * @param value zpráva
	 */
	template <typename T>
	void LogError(T msg);

	/**
	 * @brief Vypíše zprávu úrovně "Debug" na stdout pokud je definován _loggerDebug
	 * 
	 * @param value zpráva
	 */
	template <typename T>
	void LogDebug(T msg);
} // namespace Logger
