/**
 * @file netflow_exporter.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Netflow exporter. Čte data ze zadaného pcap souboru a zasílá
 * je na zadaný netflow kolektor.
 */

#pragma once

#include "pcap_reader.h"
#include "netflow_collector_connection.h"
#include "netflow_datagram.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace Netflow
{
	namespace Constants
	{
		/// @brief ICMP ip protokol číslo
		constexpr std::uint8_t IcmpProtocolNumber = 1;

		/// @brief TCP ip protokol číslo
		constexpr std::uint8_t TcpProtocolNumber = 6;

		/// @brief UDP ip protokol číslo
		constexpr std::uint8_t UdpProtocolNumber = 17;
	} // namespace Constants

	/**
	 * @brief Verze IP
	 */
	enum class IpVersion
	{
		None,
		Ipv4,
		Ipv6
	};

	/**
	 * @brief Podporované ip protokoly
	 */
	enum class Protocol
	{
		None,
		Tcp,
		Udp,
		Icmp
	};

	/**
	 * @brief Pomocná struktura pro uložení ukazatelů na začátky headerů.
	 */
	struct ParsedPacket
	{
		const EthernetHeader * eth {nullptr};

		IpVersion ipVersion {IpVersion::None};
		union {
			const Ipv4Header * ipv4h {nullptr};
			const Ipv6Header * ipv6h;
		};

		Protocol protocol {Protocol::None};
		union
		{
			const TcpHeader * tcph {nullptr};
			const UdpHeader * udph;
			const IcmpHeader * icmph;
		};

		const std::uint8_t * payload {nullptr};
		uint32_t size; ///< Celková velikost paketu
	};

	/**
	 * @brief Flow záznam. Neobsahuje všechny hodnoty Netflow V5 záznamu.
	 * Před odesláním je nutné ho převést na NetflowV5FlowRecord.
	 */
	struct FlowRecord
	{
		uint32_t srcAddr;
		uint32_t dstAddr;
		uint32_t dPkts;
		uint32_t dOctets;
		uint32_t first;
		uint32_t last;
		uint32_t srcPort;
		uint32_t dstPort;
		uint8_t tcpFlags {};
		uint8_t prot;
		uint8_t tos;

		/**
		 * @brief Porovnání s paketem
		 * 
		 * @param pkt paket
		 * @return true paket a hodnoty v struktuře jsou stejné
		 * @return false paket a hodnoty v struktuře nejsou stejné
		 */
		bool PacketEq(const ParsedPacket & pkt) const;
	};

	/**
	 * @brief Netflow exportér, který ze zachycených síťových dat ve formátu pcap vytvoří záznamy NetFlow, které odešle na kolektor.
	 */
	class NetflowExporter
	{
	public:
		/**
		 * @brief Konstruktor
		 * 
		 * @param file jméno analyzovaného souboru nebo STDIN
		 * @param collectorIp IP adresa, nebo hostname NetFlow kolektoru
		 * @param collectorPort UDP port netflow kolektoru
		 * @param activeTimer interval v sekundách, po kterém se exportují aktivní záznamy na kolektor
		 * @param interval interval v sekundách, po jehož vypršení se exportují neaktivní záznamy na kolektor
		 * @param flowCacheSize velikost flow-cache. Při dosažení max. velikosti dojde k exportu nejstaršího záznamu v cachi na kolektor
		 */
		NetflowExporter(const std::string & file, const std::string & collectorIp, std::uint32_t collectorPort, std::uint32_t activeTimer,
			std::uint32_t interval, std::uint32_t flowCacheSize);

		/**
		 * @brief Spustí netflow exportér
		 */
		void Run();
	private:
		/// @brief Ip adresa/hostname netflow koletoru
		const std::string & _collectorIp;

		/// @brief Port netflow kolektoru
		const std::uint32_t _collectorPort;

		/// @brief Netflow active timer
		const std::uint32_t _activeTimer;

		/// @brief Netflow interval
		const std::uint32_t _interval;

		/// @brief Netflow velikost flow cache
		const std::uint32_t _flowCacheSize;

		/// @brief Reader pro čtení pcap souboru/čtení z stdin
		Reader _reader;

		/// @brief Třída pro inicializaci socketu a zasílání dat do kolektoru
		CollectorConnection _collector;

		/// @brief Čas příchodu prvního packetu - představuje čas startu exporteru
		timeval _startTs;

		/// @brief Současný čas
		timeval _currentTime;

		/// @brief Počet flow záznamů, které jsme odeslali
		uint32_t _nFlowsSeen = 0;

		/// @brief Existující flow záznamy. Vector - stejně bude nutné procházet všechny při příchodu nového záznamu
		std::vector<FlowRecord> _flows;

		/// @brief Záznamy připravené k exportu
		std::vector<NetflowV5FlowRecord> _toExport;

		/**
		 * @brief Hlavní smyčka - získávání dat z `_reader` a zasílání do kolektoru
		 */
		void Loop();

		/**
		 * @brief Naparsuje paket
		 * 
		 * @param packet paket
		 * @return ParsedPacket zpracovaný packet
		 */
		ParsedPacket ParsePacket(const std::uint8_t * packet);

		/**
		 * @brief Přidá nový záznam do existujícího flow záznamu nebo vytvoří nový
		 * 
		 * @param pkt paket
		 */
		void AddNewRecord(const ParsedPacket & pkt);

		/**
		 * @brief Aktualizuje hodnoty ve flow záznamu daty z příchozího paketu
		 * 
		 * @param pkt paket
		 * @param record záznam
		 */
		void AddPacketToFlow(const ParsedPacket & pkt, FlowRecord & record);

		/**
		 * @brief Vytvoří nový flow záznam z paketu a přidá ho do `_flows`
		 * 
		 * @param pkt paket
		 */
		void CreateNewFlow(const ParsedPacket & pkt);

		/**
		 * @brief Zkontroluje timery (inactive + active) a pokud je to nutné, tak záznam připraví k exportu (SaveFlowExport())
		 * 
		 * @param record záznam
		 * @return true záznam byl připraven k exportu
		 * @return false záznam nebyl připraven k exportu
		 */
		bool TryFlowExport(FlowRecord & record);

		/**
		 * @brief Uloží záznam do `_toExport` - připraven k exportu
		 * 
		 * @param record záznam
		 */
		void SaveFlowExport(FlowRecord & record);

		/**
		 * @brief Exportuje flow záznamy uložené v `_toExport` na kolektor
		 */
		void ExportFlows();

		/**
		 * @brief Spočítá IPv4 checksum
		 * 
		 * @param iph ipv4 header
		 * @return std::uint32_t checksum
		 */
		std::uint32_t CalculateIpChecksum(const Ipv4Header * iph);

		/**
		 * @brief Převede timeval na sekundy
		 * 
		 * @param t timeval
		 * @return uint32_t sekundy
		 */
		static double TimevalToSec(const timeval & t);
	};
} // namespace Netflow
