/**
 * @file pcap_reader.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Třída pro čtení z pcap souborů
 */

#pragma once

#include <pcap.h>

#include "pcap_utils.h"

#include <string>
#include <cstdint>

namespace Netflow
{
	/**
	 * @brief Paket přečtený z pcap souboru
	 */
	struct Packet
	{
		pcap_pkthdr * pktHeader {nullptr};      ///< Metadata z pcap souboru
		const std::uint8_t * pktData {nullptr}; ///< Data paketu
	};

	/**
	 * @brief Třída pro čtení pcap souborů.
	 */
	class Reader
	{
	public:
		/**
		 * @brief Konstruktor
		 * 
		 * @param file soubor, ze kterého číst nebo "-", který značí stdin
		 */
		Reader(const std::string & file);

		/**
		 * @brief Vrátí další paket z pcap souboru/stdin
		 * 
		 * @return Packet paket
		 */
		Packet GetNextPacket();

		/**
		 * @brief Vrátí počet přečtených paketů
		 * 
		 * @return std::uint64_t počet přečtených paketů
		 */
		std::uint64_t GetPacketCount();
	private:
		/// @brief Název souboru (prázdný pro stdin)
		const std::string _file;

		/// @brief Počet přečtených paketů
		std::uint64_t _packetCount = 0;

		/// @brief Pcap soubor / stdin stream
		pcap_t * _pcap;

		/**
		 * @brief Inicializuje `_pcap` otevřením souboru `_file` nebo stdin
		 */
		void OpenPcapFile();
	};
} // namespace Netflow
