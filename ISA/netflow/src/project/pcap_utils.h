/**
 * @file pcap_utils.h
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Pomocné struktury pro pcap.
 * Zdroj: https://www.tcpdump.org/pcap.html (upraveno pro c++)
 */

#pragma once

#include <cstdint>
#include <string>
#include <bitset>
#include <iostream>

#include <netinet/in.h>

#include <pcap.h>

namespace
{
	std::string Int8ToHex(uint8_t x)
	{
		static char arr[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };
		return { arr[(x >> 4) & 0x0F], arr[x & 0x0F] };
	}

	std::string Int16ToHex(uint16_t x)
	{
		return Int8ToHex(static_cast<uint8_t>(x >> 8)) + Int8ToHex(static_cast<uint8_t>(x));
	}

	std::string Int32ToHex(uint32_t x)
	{
		return Int16ToHex(static_cast<uint16_t>(x >> 16)) + Int16ToHex(static_cast<uint16_t>(x));
	}
}


namespace Netflow
{
	using namespace std;

	static std::string IntToIpv4(uint32_t ip)
	{
		return std::to_string(ip >> 24) + "." + std::to_string((ip >> 16) & 0x000000FF)
				+ "." + std::to_string((ip >> 8) & 0x000000FF) + "." + std::to_string(ip & 0x000000FF);
	};

	/// @brief Velikost ethernet headeru v bytech
	constexpr uint32_t EthernetHeaderSize = 14;

	/// @brief Velikost ipv6 headeru v bytech
	constexpr uint32_t Ipv6HeaderSize = 40;

	/// @brief Velikost ethernet adresy v bytech
	constexpr uint32_t EthAddrLen = 6;

	/// Ip header flagy
	constexpr uint16_t Ip_Rf = 0x8000; ///< "reserved fragment flag"
	constexpr uint16_t Ip_Df = 0x4000; ///< "don't fragment flag"
	constexpr uint16_t Ip_Mf = 0x2000; ///< "more fragments flag"
	constexpr uint16_t Ip_Offmask = 0x1FFF; ///< "mask for fragmenting bits"

	/// Tcp header flagy
	constexpr uint8_t Th_FIN = 0x01;
	constexpr uint8_t Th_SYN = 0x02;
	constexpr uint8_t Th_RST = 0x04;
	constexpr uint8_t Th_PUSH = 0x08;
	constexpr uint8_t Th_ACK = 0x10;
	constexpr uint8_t Th_URG = 0x20;
	constexpr uint8_t Th_ECE = 0x40;
	constexpr uint8_t Th_CWR = 0x80;
	constexpr uint8_t Th_Flags = (Th_FIN|Th_SYN|Th_RST|Th_ACK|Th_URG|Th_ECE|Th_CWR);

	/// EtherType pro Ipv4 a Ipv6
	constexpr uint32_t EtherTypeIpv4 = 0x0800;
	constexpr uint32_t EtherTypeIpv6 = 0x86DD;

	/**
	 * @brief Ethernet header.
	 * Některé hodnoty nejsou ve správném pořadí - je potřeba přehodit byte order. K tomu slouží metody této struktury.
	 */
	struct EthernetHeader
	{
		uint8_t destHost[EthAddrLen]; ///< "Destination host address"
		uint8_t srcHost[EthAddrLen];  ///< "Source host address"
		uint8_t type[2];              ///< "EtherType"

		uint16_t Type() const
		{
			return ntohs(static_cast<uint16_t>(type[0]) | (static_cast<uint16_t>(type[1]) << 8));
		}

		/**
		 * @brief ToString metoda (pro debug)
		 * 
		 * @return std::string string reprezentace této struktury
		 */
		std::string ToString() const
		{
			std::string str = "EthernetHeader { destHost: \"";
			for (std::uint8_t i = 0; i < EthAddrLen; i++) {
				str += Int8ToHex(destHost[i]);
				if (i != EthAddrLen-1) {
					str += "-";
				}
			}
			str += "\", srcHost: \"";
			for (std::uint8_t i = 0; i < EthAddrLen; i++) {
				str += Int8ToHex(srcHost[i]);
				if (i != EthAddrLen-1) {
					str += "-";
				}
			}
			str += "\", type: \"0x" + Int16ToHex(Type()) + "\" }";

			return str;
		}
	};

	/**
	 * @brief Ipv4 header.
	 * Některé hodnoty nejsou ve správném pořadí - je potřeba přehodit byte order. K tomu slouží metody této struktury.
	 */
	struct Ipv4Header
	{
		uint8_t v_ihl;           ///< (0-3) "version"; (4-7) "internet header length"
		uint8_t dscp_ecn;        ///< (8-13) "DSCP (TOS)"; (14-15) "ECN"
		uint8_t len[2];          ///< (16-31) "total length"
		uint8_t id[2];           ///< (32-47) "identification"
		uint8_t flags_offset[2]; ///< (48-50) "flags"; (51-63) "fragment offset"
		uint8_t ttl;             ///< (64-71) "time to live"
		uint8_t prot;            ///< (72-79) "protocol"
		uint8_t sum[2];          ///< (80-95) "header checksum"
		uint8_t srcAddr[4];      ///< (96-127) "source address"
		uint8_t dstAddr[4];      ///< (128-159) "destination address"

		uint8_t Version() const
		{
			return (v_ihl >> 4) & 0x0F;
		}

		uint8_t Ihl() const
		{
			return v_ihl & 0x0F;
		}

		uint8_t Dscp() const
		{
			return (dscp_ecn >> 6) & 0x03;
		}

		uint8_t Ecn() const
		{
			return dscp_ecn & 0x3F;
		}

		uint16_t Length() const
		{
			return ntohs(static_cast<uint16_t>(len[0]) | (static_cast<uint16_t>(len[1]) << 8));
		}

		uint16_t Id() const
		{
			return ntohs(static_cast<uint16_t>(id[0]) | (static_cast<uint16_t>(id[1]) << 8));
		}

		uint8_t Flags() const
		{
			return static_cast<uint8_t>((ntohs(static_cast<uint16_t>(flags_offset[0]) | (static_cast<uint16_t>(flags_offset[1]) << 8)) >> 13) & 0x07);
		}

		uint16_t Offset() const
		{
			return static_cast<uint8_t>(ntohs(static_cast<uint16_t>(flags_offset[0]) | (static_cast<uint16_t>(flags_offset[1]) << 8)) & 0x1F);
		}

		uint16_t Checksum() const
		{
			return ntohs(static_cast<uint16_t>(sum[0]) | (static_cast<uint16_t>(sum[1]) << 8));
		}

		uint32_t SrcAddr() const
		{
			return ntohl(static_cast<uint32_t>(srcAddr[0]) | (static_cast<uint32_t>(srcAddr[1]) << 8)
				| (static_cast<uint32_t>(srcAddr[2]) << 16) | (static_cast<uint32_t>(srcAddr[3]) << 24));
		}

		uint32_t DstAddr() const
		{
			return ntohl(static_cast<uint32_t>(dstAddr[0]) | (static_cast<uint32_t>(dstAddr[1]) << 8)
				| (static_cast<uint32_t>(dstAddr[2]) << 16) | (static_cast<uint32_t>(dstAddr[3]) << 24));
		}

		/**
		 * @brief ToString metoda (pro debug)
		 * 
		 * @return std::string string reprezentace této struktury
		 */
		std::string ToString() const
		{
			return "Ipv4Header: { version: \"" + std::to_string(Version())
				+ "\", ihl: \"" + std::to_string(Ihl())
				+ "\", dscp: \"" + std::to_string(Dscp())
				+ "\", ecn: \"" + std::to_string(Ecn())
				+ "\", len: \"" + std::to_string(Length())
				+ "\", id: \"0x" + Int16ToHex(Id())
				+ "\", flags: \"0x" + Int8ToHex(Flags())
				+ "\", off: \"0x" + Int16ToHex(Offset())
				+ "\", ttl: \"" + std::to_string(ttl)
				+ "\", prot: \"" + std::to_string(prot)
				+ "\", sum: \"" + std::to_string(Checksum())
				+ "\", srcAddr: \"" + std::to_string(srcAddr[0]) + "." + std::to_string(srcAddr[1])
				+ "." + std::to_string(srcAddr[2]) + "." + std::to_string(srcAddr[3])
				+ "\", dstAddr: \"" + std::to_string(dstAddr[0]) + "." + std::to_string(dstAddr[1])
				+ "." + std::to_string(dstAddr[2]) + "." + std::to_string(dstAddr[3])
				+ "\" }";
		}
	};

	/**
	 * @brief Ipv6 header
	 * Některé hodnoty nejsou ve správném pořadí - je potřeba přehodit byte order. K tomu slouží metody této struktury.
	 */
	struct Ipv6Header
	{
		uint8_t v_dscp_ecn_fl[4]; ///< (0-3) "Version"; (4-9) "DSCP (TOS)"; (10-11) "ECN"; (12-31) "Flow label"
		uint8_t payloadLen[2];    ///< (32-47) "Payload length"
		uint8_t next;             ///< (48-55) "Next header"
		uint8_t hopLimit;         ///< (56-63) "Hop limit"
		uint8_t srcAddr[16];      ///< (64-191) "Source address"
		uint8_t dstAddr[16];      ///< (192-255) "Destination address"

		uint8_t Version() const
		{
			return (v_dscp_ecn_fl[0] >> 4) & 0x0F;
		}

		uint8_t Dscp() const
		{
			return (v_dscp_ecn_fl[0] & 0x0F) | (v_dscp_ecn_fl[1] & 0x30);
		}

		uint8_t Ecn() const
		{
			return (v_dscp_ecn_fl[1] & 0xC0) >> 2;
		}

		uint32_t FlowLabel() const
		{
			return ((v_dscp_ecn_fl[1] & 0x0000'000F) << 16) | (v_dscp_ecn_fl[2] << 8) | (v_dscp_ecn_fl[3]);
		}

		uint16_t PayloadLength() const
		{
			return ntohs(static_cast<uint16_t>(payloadLen[0]) | (static_cast<uint16_t>(payloadLen[1]) << 8));
		}

		/**
		 * @brief ToString metoda (pro debug)
		 * 
		 * @return std::string string reprezentace této struktury
		 */
		std::string ToString() const
		{
			std::string flowLabel = Int8ToHex((FlowLabel() & 0x000F'0000) >> 16)
				+ Int8ToHex((FlowLabel() & 0x0000'FF00) >> 8)
				+ Int8ToHex(FlowLabel() & 0x0000'00FF);
			
			return "Ipv6Header: { version: \"" + std::to_string(Version())
				+ "\", dscp: \"" + std::to_string(Dscp())
				+ "\", ecn: \"" + std::to_string(Ecn())
				+ "\", flowlabel: \"0x" + flowLabel
				+ "\", payloadLen: \"" + std::to_string(PayloadLength())
				+ "\", next: \"" + std::to_string(next)
				+ "\", hopLimit: \"" + std::to_string(hopLimit)
				+ "\", srcAddr: \"" + Int8ToHex(srcAddr[0]) + Int8ToHex(srcAddr[1]) + ":" + Int8ToHex(srcAddr[2]) + Int8ToHex(srcAddr[3])
				+ ":" + Int8ToHex(srcAddr[4]) + Int8ToHex(srcAddr[5]) + ":" + Int8ToHex(srcAddr[6]) + Int8ToHex(srcAddr[7])
				+ ":" + Int8ToHex(srcAddr[8]) + Int8ToHex(srcAddr[9]) + ":" + Int8ToHex(srcAddr[10]) + Int8ToHex(srcAddr[11])
				+ ":" + Int8ToHex(srcAddr[12]) + Int8ToHex(srcAddr[13]) + ":" + Int8ToHex(srcAddr[14]) + Int8ToHex(srcAddr[15])
				+ "\", dstAddr: \"" + Int8ToHex(dstAddr[0]) + Int8ToHex(dstAddr[1]) + ":" + Int8ToHex(dstAddr[2]) + Int8ToHex(dstAddr[3])
				+ ":" + Int8ToHex(dstAddr[4]) + Int8ToHex(dstAddr[5]) + ":" + Int8ToHex(dstAddr[6]) + Int8ToHex(dstAddr[7])
				+ ":" + Int8ToHex(dstAddr[8]) + Int8ToHex(dstAddr[9]) + ":" + Int8ToHex(dstAddr[10]) + Int8ToHex(dstAddr[11])
				+ ":" + Int8ToHex(dstAddr[12]) + Int8ToHex(dstAddr[13]) + ":" + Int8ToHex(dstAddr[14]) + Int8ToHex(dstAddr[15])
				+ "\" }";
		}
	};

	/**
	 * @brief TCP header.
	 * Některé hodnoty nejsou ve správném pořadí - je potřeba přehodit byte order. K tomu slouží metody této struktury.
	 * Zdroj: https://www.tcpdump.org/pcap.html
	 */
	struct TcpHeader
	{
		uint8_t srcPort[2]; ///< (0-15) "source port"
		uint8_t dstPort[2]; ///< (16-31) "destination port"
		uint8_t seq[4];     ///< (32-63) "sequence number"
		uint8_t ack[4];     ///< (64-95) "acknowledgement number"
		uint8_t offx2;      ///< (96-103) "data offset, reserved, NS flag"
		uint8_t flags;      ///< (104-111) "flags"
		uint8_t win[2];     ///< (112-127) "window"
		uint8_t sum[2];     ///< (128-143) "checksum"
		uint8_t urp[2];     ///< (144-159) "urgent pointer"

		uint16_t SrcPort() const
		{
			return ntohs(static_cast<uint16_t>(srcPort[0]) | (static_cast<uint16_t>(srcPort[1]) << 8));
		};

		uint16_t DstPort() const
		{
			return ntohs(static_cast<uint16_t>(dstPort[0]) | (static_cast<uint16_t>(dstPort[1]) << 8));
		};

		uint32_t Seq() const
		{
			return ntohl(static_cast<uint32_t>(seq[0]) | (static_cast<uint32_t>(seq[1]) << 8)
				| (static_cast<uint32_t>(seq[2]) << 16) | (static_cast<uint32_t>(seq[3]) << 24));
		};

		uint32_t Ack() const
		{
			return ntohl(static_cast<uint32_t>(ack[0]) | (static_cast<uint32_t>(ack[1]) << 8)
				| (static_cast<uint32_t>(ack[2]) << 16) | (static_cast<uint32_t>(ack[3]) << 24));
		};

		uint16_t Win() const
		{
			return ntohs(static_cast<uint16_t>(win[0]) | (static_cast<uint16_t>(win[1]) << 8));
		};

		uint16_t Sum() const
		{
			return ntohs(static_cast<uint16_t>(sum[0]) | (static_cast<uint16_t>(sum[1]) << 8));
		};

		uint16_t Urp() const
		{
			return ntohs(static_cast<uint16_t>(urp[0]) | (static_cast<uint16_t>(urp[1]) << 8));
		};

		uint8_t GetOffset() const
		{
			return (offx2 & 0xF0) >> 4;
		};

		/**
		 * @brief ToString metoda (pro debug)
		 * 
		 * @return std::string string reprezentace této struktury
		 */
		std::string ToString() const
		{
			return "TcpHeader: { srcPort: \"" + std::to_string(SrcPort())
				+ "\", dstPort: \"" + std::to_string(DstPort())
				+ "\", seq: \"" + Int32ToHex(Seq())
				+ "\", ack: \"" + Int32ToHex(Ack())
				+ "\", headerLength: \"" + std::to_string(GetOffset())
				+ "\", flags: \"" + std::bitset<8>(flags).to_string()
				+ "\", win: \"" + std::to_string(Win())
				+ "\", sum: \"" + std::to_string(Sum())
				+ "\", urp: \"" + std::to_string(Urp())
				+ "\" }";
		}
	};

	/**
	 * @brief UDP header
	 * Některé hodnoty nejsou ve správném pořadí - je potřeba přehodit byte order. K tomu slouží metody této struktury.
	 */
	struct UdpHeader
	{
		uint8_t srcPort[2];  ///< (0-15) "source port"
		uint8_t dstPort[2];  ///< (16-31) "destination port"
		uint8_t len[2];      ///< (32-47) "udp length"
		uint8_t checksum[2]; ///< (48-63) "udp checksum"

		uint16_t SrcPort() const
		{
			return ntohs(static_cast<uint16_t>(srcPort[0]) | (static_cast<uint16_t>(srcPort[1]) << 8));
		}

		uint16_t DstPort() const
		{
			return ntohs(static_cast<uint16_t>(dstPort[0]) | (static_cast<uint16_t>(dstPort[1]) << 8));
		}
		
		uint16_t Length() const
		{
			return ntohs(static_cast<uint16_t>(len[0]) | (static_cast<uint16_t>(len[1]) << 8));
		}
		
		uint16_t Checksum() const
		{
			return ntohs(static_cast<uint16_t>(checksum[0]) | (static_cast<uint16_t>(checksum[1]) << 8));
		}

		/**
		 * @brief ToString metoda (pro debug)
		 * 
		 * @return std::string string reprezentace této struktury
		 */
		std::string ToString() const
		{
			return "UdpHeader: { srcPort: \"" + std::to_string(SrcPort())
				+ "\", dstPort: \"" + std::to_string(DstPort())
				+ "\", len: \"" + std::to_string(Length())
				+ "\", checksum: \"" + std::to_string(Checksum())
				+ "\" }";
		}
	};

	/**
	 * @brief ICMP header.
	 * Některé hodnoty nejsou ve správném pořadí - je potřeba přehodit byte order. K tomu slouží metody této struktury.
	 */
	struct IcmpHeader
	{
		uint8_t type;        ///< (0-7) ICMP type
		uint8_t code;        ///< (8-15) "ICMP subtype"
		uint8_t checksum[2]; ///< (16-31) "internet checksum"
		uint8_t roh[4];      ///< (32-63) "rest of header"

		uint16_t Checksum() const
		{
			return ntohs(static_cast<uint16_t>(checksum[0]) | (static_cast<uint16_t>(checksum[1]) << 8));
		}

		uint32_t RestOfHeader() const
		{
			return ntohl(static_cast<uint32_t>(roh[0]) | (static_cast<uint32_t>(roh[1]) << 8)
				| (static_cast<uint32_t>(roh[2]) << 16) | (static_cast<uint32_t>(roh[3]) << 24));
		}

		/**
		 * @brief ToString metoda (pro debug)
		 * 
		 * @return std::string string reprezentace této struktury
		 */
		std::string ToString() const
		{
			return "UdpHeader: { type: \"" + std::to_string(type)
				+ "\", code: \"" + std::to_string(code)
				+ "\", checksum: \"" + std::to_string(Checksum())
				+ "\", roh: \"" + std::to_string(RestOfHeader())
				+ "\" }";
		}
	};
} // namespace Netflow
