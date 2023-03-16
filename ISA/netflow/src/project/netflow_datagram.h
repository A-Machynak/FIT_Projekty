/**
 * @file netflow_datagram.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Struktury pro netflow V5 hlavičku a flow záznam
 */

#pragma once

#include <cstdint>
#include <array>
#include <netinet/in.h>

namespace
{
	/// Nekoukat. Serializace 2 a 4 bytů. Spoleháme na optimalizaci překladačem
	#define S2B(v) static_cast<uint8_t>(htons(v)), static_cast<uint8_t>(htons(v) >> 8)
	#define S4B(v) static_cast<uint8_t>(htonl(v)), static_cast<uint8_t>(htonl(v) >> 8), static_cast<uint8_t>(htonl(v) >> 16), static_cast<uint8_t>(htonl(v) >> 24)
} // namespace


namespace Netflow
{
	using namespace std;

	/// @brief Velikost netflow hlavičky
	constexpr uint32_t NetflowV5HeaderSize = 24;

	/// @brief Velikost netflow záznamu
	constexpr uint32_t NetflowV5RecordSize = 48;

	/**
	 * @brief Netflow V5 hlavička
	 */
	struct NetflowV5Header
	{
		const uint16_t version {5};   ///< (0-1)   - "NetFlow export format version number"
		uint16_t count {};            ///< (2-3)   - "Number of flows exported in this packet (1-30)"
		uint32_t sysUptime {};        ///< (4-7)   - "Current time in milliseconds since the export device booted"
		uint32_t unixSecs {};         ///< (8-11)  - "Current count of seconds since 0000 UTC 1970"
		uint32_t unixNsecs {};        ///< (12-15) - "Residual nanoseconds since 0000 UTC 1970"
		uint32_t flowSequence {};     ///< (16-19) - "Sequence counter of total flows seen"
		uint8_t engineType {};        ///< (20)    - "Type of flow-switching engine"
		uint8_t engineId {};          ///< (21)    - "Slot number of the flow-switching engine"
		uint16_t samplingInterval {}; ///< (22-23) - "First two bits hold the sampling mode; remaining 14 bits hold value of sampling interval"

		/**
		 * @brief Velikost této struktury (konstantní)
		 * 
		 * @return uint32_t velikost
		 */
		uint32_t Size() const
		{
			return NetflowV5HeaderSize; // konstantní velikost
		}

		/**
		 * @brief Serializace této struktury vhodná pro zaslání po síti
		 * 
		 * @return std::array<uint8_t, NetflowV5HeaderSize> serializace této struktury
		 */
		std::array<uint8_t, NetflowV5HeaderSize> Serialize() const
		{
			// Zarovnání není zaručené, musíme ručně
			return {
				S2B(version),
				S2B(count),
				S4B(sysUptime),
				S4B(unixSecs),
				S4B(unixNsecs),
				S4B(flowSequence),
				engineType,
				engineId,
				S2B(samplingInterval)
			};
		}
	};

	/**
	 * @brief Netflow V5 flow záznam
	 */
	struct NetflowV5FlowRecord
	{
		uint32_t srcAddr {}; ///< (0-3)   - "Source IP address"
		uint32_t dstAddr {}; ///< (4-7)   - "Destination IP address"
		uint32_t nextHop {}; ///< (8-11)  - "IP address of next hop router" (nepoužito, neznáme)
		uint16_t input {};   ///< (12-13) - "SNMP index of input interface" (nepoužito, neznáme)
		uint16_t output {};  ///< (14-15) - "SNMP index of output interface" (nepoužito, neznáme)
		uint32_t dPkts {};   ///< (16-19) - "Packets in the flow"
		uint32_t dOctets {}; ///< (20-23) - "Total number of Layer 3 bytes in the packets of the flow"
		uint32_t first {};   ///< (24-27) - "SysUptime at start of flow"
		uint32_t last {};    ///< (28-31) - "SysUptime at the time the last packet of the flow was received"
		uint16_t srcPort {}; ///< (32-33) - "TCP/UDP source port number or equivalent"
		uint16_t dstPort {}; ///< (34-35) - "TCP/UDP destination port number or equivalent"
		uint8_t pad1 {};     ///< (36)    - "Unused (zero) bytes"
		uint8_t tcpFlags {}; ///< (37)    - "Cumulative OR of TCP flags"
		uint8_t prot {};     ///< (38)    - "IP protocol type (for example, TCP = 6; UDP = 17)"
		uint8_t tos {};      ///< (39)    - "IP type of service (ToS)"
		uint16_t srcAs {};   ///< (40-41) - "Autonomous system number of the source, either origin or peer" (nepoužito, neznáme)
		uint16_t dstAs {};   ///< (42-43) - "Autonomous system number of the destination, either origin or peer" (nepoužito, neznáme)
		uint8_t srcMask {};  ///< (44)    - "Source address prefix mask bits" (nepoužito, neznáme)
		uint8_t dstMask {};  ///< (45)    - "Destination address prefix mask bits" (nepoužito, neznáme)
		uint16_t pad2 {};    ///< (46-47) - "Unused (zero) bytes"

		/**
		 * @brief Velikost této struktury (konstantní)
		 * 
		 * @return uint32_t velikost
		 */
		uint32_t Size() const
		{
			return NetflowV5RecordSize; // konstantní velikost
		}

		/**
		 * @brief Serializace této struktury vhodná pro zaslání po síti
		 * 
		 * @return std::array<uint8_t, NetflowV5RecordSize> serializace této struktury
		 */
		std::array<uint8_t, NetflowV5RecordSize> Serialize() const
		{
			// Zarovnání není zaručené, musíme ručně
			return {
				S4B(srcAddr),
				S4B(dstAddr),
				S4B(nextHop),
				S2B(input),
				S2B(output),
				S4B(dPkts),
				S4B(dOctets),
				S4B(first),
				S4B(last),
				S2B(srcPort),
				S2B(dstPort),
				pad1,
				tcpFlags,
				prot,
				tos,
				S2B(srcAs),
				S2B(dstAs),
				srcMask,
				dstMask,
				S2B(pad2)
			};
		}
	};
} // namespace Netflow
