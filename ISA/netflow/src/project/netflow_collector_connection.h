/**
 * @file netflow_collector_connection.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Třída pro komunikaci s netflow kolektorem
 */

#pragma once

#include <cstdint>
#include <string>
#include <array>

#include <sys/socket.h>

namespace Netflow
{
	/**
	 * @brief Třída sloužící pro vytvoření soketu na kolektor. Komunikace probíhá přes UPD.
	 */
	class CollectorConnection
	{
	public:
		/**
		 * @brief Konstruktor
		 * 
		 * @param collectorIp Ip kolektoru
		 * @param port Port kolektoru
		 */
		CollectorConnection(const std::string & collectorIp, std::uint32_t port);

		/**
		 * @brief Destruktor - uzavření soketu
		 */
		~CollectorConnection();

		/**
		 * @brief Zašle zprávu na kolektor
		 * 
		 * @param data data zprávy
		 * @param len délka zprávy
		 * @return true zpráva byla úspěšně zaslána
		 * @return false došlo k chybě při zasílání zprávy
		 */
		bool Send(const std::uint8_t * data, int len) const;

		/**
		 * @brief Jestli proběhla inicializace v pořádku
		 * 
		 * @return true soket je inicializovaný
		 * @return false chyba při inicializaci
		 */
		bool IsInitialized() const;
	private:
		/// @brief Socket fd
		int _sockfd;

		/// @brief Inicializační flag
		bool _initialized;

		/// @brief Adresa kolektoru
		sockaddr _collectorAddr;
		socklen_t _collectorAddrLen;

		/**
		 * @brief Inicializace socketu
		 * 
		 * @param collectorIp ip kolektoru
		 * @param collectorPort port kolektoru
		 * @return true inicializace proběhla v pořádku
		 * @return false chyba při inicializaci
		 */
		bool InitConnection(const char * collectorIp, const char * collectorPort);
	};
} // namespace Netflow
