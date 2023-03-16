/**
 * @file netflow_exporter.cpp
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Implementace
 */

#include "netflow_exporter.h"

#include <chrono>

//#define _loggerDebug
#include "logger/logger.hpp"

namespace Netflow
{
	bool FlowRecord::PacketEq(const ParsedPacket & pkt) const
	{
		return srcAddr == pkt.ipv4h->SrcAddr()
			&& dstAddr == pkt.ipv4h->DstAddr()
			&& prot == pkt.ipv4h->prot
			&& tos == pkt.ipv4h->Dscp()
			&& srcPort == pkt.tcph->SrcPort()
			&& dstPort == pkt.tcph->DstPort();
	}

	NetflowExporter::NetflowExporter(const std::string & file, const std::string & collectorIp, std::uint32_t collectorPort,
			std::uint32_t activeTimer, std::uint32_t interval, std::uint32_t flowCacheSize)
		: _reader(file), _collectorIp(collectorIp), _collectorPort(collectorPort), _activeTimer(activeTimer), _interval(interval),
		_flowCacheSize(flowCacheSize), _collector(collectorIp, collectorPort)
	{
		_toExport.reserve(30);
		_flows.reserve(flowCacheSize);
	}

	void NetflowExporter::Run()
	{
		Loop();
	}

	void NetflowExporter::Loop()
	{
		if (!_collector.IsInitialized()) {
			return;
		}

		Logger::LogInfo<>("Start");

		Packet p = _reader.GetNextPacket();
		if (p.pktData != nullptr && p.pktHeader != nullptr) {
			_startTs = p.pktHeader->ts;
		}

		do {
			// vyprázdníme záznamy k exportu
			_toExport.clear();

			_currentTime = p.pktHeader->ts;
			auto parsed = ParsePacket(p.pktData);
			parsed.size = p.pktHeader->len;

			if (parsed.eth && parsed.ipVersion == IpVersion::Ipv4) {
				// netflow v5 podporuje pouze ipv4
				AddNewRecord(parsed);
			}

			// zkontrolujeme, jestli nemáme uloženo příliš záznamů
			if (_flows.size() >= _flowCacheSize) {
				// exportujeme první záznam
				SaveFlowExport(_flows[0]);
				_flows.erase(_flows.begin());
			}

			// exportujeme to, co je v `_toExport`
			ExportFlows();
			p = _reader.GetNextPacket();
		} while(p.pktHeader != nullptr && p.pktData != nullptr);

		// exportujeme zbývající záznamy
		for (auto & flow : _flows) {
			SaveFlowExport(flow);
		}
		ExportFlows();
		_flows.clear();

		Logger::LogInfo<>("Finished reading. Exiting...");
	}

	ParsedPacket NetflowExporter::ParsePacket(const std::uint8_t * packet)
	{
		constexpr std::uint32_t ValidChecksum = 0x0000FFFF;

		ParsedPacket pkt;
		pkt.eth = reinterpret_cast<const EthernetHeader *>(packet);
		
		std::uint32_t currentOffset = EthernetHeaderSize;

		// zkusíme převést na ipv4
		const auto ip = reinterpret_cast<const Ipv4Header *>(packet + currentOffset);
		std::uint32_t protocol = 0;

		// zjistíme, jestli se jedná o ipv4 nebo ipv6
		if (pkt.eth->Type() == EtherTypeIpv4) {
			// nejspíše se jedná o ipv4; zkontrolujeme IHL a checksum
			const auto sizeIp = ip->Ihl() * 4;

			if (sizeIp < 20) {
				Logger::LogDebug<>("Invalid ip header: sizeIp < " + std::to_string(sizeIp));
				return pkt;
			}

			// zkontrolujeme checksum; asi není potřeba
			/*const auto checksum = CalculateIpChecksum(ip);
			if (checksum != ValidChecksum) {
				Logger::LogDebug<>("Invalid IPv4 checksum value: " + std::to_string(checksum));
				return pkt;
			}*/
			pkt.ipv4h = ip;

			currentOffset += sizeIp;
			protocol = pkt.ipv4h->prot;
			pkt.ipVersion = IpVersion::Ipv4;
		}
		else if (pkt.eth->Type() == EtherTypeIpv6) {
			// jedná se o ipv6; cast
			pkt.ipv6h = reinterpret_cast<const Ipv6Header *>(packet + currentOffset);

			currentOffset += Ipv6HeaderSize;
			protocol = pkt.ipv6h->next;
			pkt.ipVersion = IpVersion::Ipv6;
		}
		else {
			Logger::LogDebug<>("Not an ipv4/ipv6 header: " + std::to_string(ip->Ihl()));
			return pkt;
		}
		
		// tcp/udp/icmp?
		if (protocol == Constants::TcpProtocolNumber) {
			const auto tcp = reinterpret_cast<const TcpHeader *>(packet + currentOffset);
			const auto sizeTcp = tcp->GetOffset() * 4;

			if (sizeTcp < 20 || sizeTcp > 60) {
				Logger::LogDebug<>("Invalid TCP header size: " + std::to_string(sizeTcp));
				return pkt;
			}
			currentOffset += sizeTcp;
			pkt.tcph = tcp;
			pkt.protocol = Protocol::Tcp;
		}
		else if(protocol == Constants::UdpProtocolNumber) {
			const auto udp = reinterpret_cast<const UdpHeader *>(packet + currentOffset);
			const auto sizeUdp = udp->Length();

			if (sizeUdp < 8) {
				Logger::LogDebug<>("Invalid UDP header size: " + std::to_string(sizeUdp));
				return pkt;
			}
			currentOffset += sizeUdp;
			pkt.udph = udp;
			pkt.protocol = Protocol::Udp;
		}
		else if(protocol == Constants::IcmpProtocolNumber) {
			pkt.icmph = reinterpret_cast<const IcmpHeader *>(packet + currentOffset);
			currentOffset += 8;
			pkt.protocol = Protocol::Icmp;
		}
		else {
			Logger::LogDebug<>("Unknown protocol: " + std::to_string(protocol));
			pkt.ipVersion = IpVersion::None;
			pkt.ipv4h = nullptr;
			return pkt;
		}

		pkt.payload = packet + currentOffset;
		return pkt;
	}

	void NetflowExporter::AddNewRecord(const ParsedPacket & pkt)
	{
		bool found = false;

		// erase-remove idiom
		_flows.erase(std::remove_if(_flows.begin(), _flows.end(),
			[this, pkt, &found](FlowRecord & record) {
				// zkusíme exportovat záznam, pokud vypršel jeden z timerů
				if (TryFlowExport(record)) {
					// záznam byl exportován a smazán
					return true;
				}

				// záznam nebyl exportován; pokud příchozí paket odpovídá již
				// existujícímu záznamu, tak jej přidáme
				if (record.PacketEq(pkt)) {
					AddPacketToFlow(pkt, record);
					found = true;
				}
				return false;
		}), _flows.end());

		if (!found) {
			// paket nebyl přidán do žádné existující flow; vytvoříme novou
			CreateNewFlow(pkt);
		}
	}

	void NetflowExporter::AddPacketToFlow(const ParsedPacket & pkt, FlowRecord & record)
	{
		record.dPkts++;
		record.dOctets += pkt.ipv4h->Length();
		record.last = TimevalToSec(_currentTime);
		if (pkt.protocol == Protocol::Tcp) {
			record.tcpFlags |= pkt.tcph->flags;
		}
	}

	void NetflowExporter::CreateNewFlow(const ParsedPacket & pkt)
	{
		FlowRecord r {};

		// packet musí být ipv4
		r.srcAddr = pkt.ipv4h->SrcAddr();
		r.dstAddr = pkt.ipv4h->DstAddr();
		r.dOctets = pkt.ipv4h->Length();
		r.dPkts = 1;
		r.first = TimevalToSec(_currentTime);
		r.last = TimevalToSec(_currentTime);
		if (pkt.protocol == Protocol::Tcp) {
			r.prot = Constants::TcpProtocolNumber;
			r.srcPort = pkt.tcph->SrcPort();
			r.dstPort = pkt.tcph->DstPort();
			r.tcpFlags = pkt.tcph->flags;
		}
		else if (pkt.protocol == Protocol::Udp) {
			r.prot = Constants::UdpProtocolNumber;
			r.srcPort = pkt.udph->SrcPort();
			r.dstPort = pkt.udph->DstPort();
		}
		else if (pkt.protocol == Protocol::Icmp) {
			r.prot = Constants::IcmpProtocolNumber;
		}
		else {
			// nepodporováno; ale můžeme vložit číslo protokolu alespoň
			r.prot = pkt.ipv4h->prot;
		}
		r.tos = pkt.ipv4h->Dscp();

		Logger::LogDebug<>("Saved record: " + IntToIpv4(r.srcAddr) + " " + IntToIpv4(r.dstAddr));
		_flows.push_back(r);
	}

	bool NetflowExporter::TryFlowExport(FlowRecord & record)
	{
		uint32_t tActive = record.last - record.first;
		uint32_t tInactive = TimevalToSec(_currentTime) - record.last;

		if (tActive >= _activeTimer || tInactive >= _interval) {
			// vypršel timer, "exportujeme" packet (přidáme ho do paketů k exportu)
			Logger::LogDebug<>("Saved flow export");
			SaveFlowExport(record);
			return true;
		}
		return false;
	}

	void NetflowExporter::SaveFlowExport(FlowRecord & record)
	{
		// header vytvoříme až při exportu
		NetflowV5FlowRecord r {};
		r.dOctets = record.dOctets;
		r.srcAddr = record.srcAddr;
		r.dstAddr = record.dstAddr;
		r.dPkts = record.dPkts;
		r.dOctets = record.dOctets;
		r.first = record.first;
		r.last = record.last;
		r.srcPort = record.srcPort;
		r.dstPort = record.dstPort;
		r.tcpFlags = record.tcpFlags;
		r.prot = record.prot;
		r.tos = record.tos;

		_toExport.push_back(r);
	}

	void NetflowExporter::ExportFlows()
	{
		// Maximální počet záznamů exportovaných v 1 odeslání
		constexpr std::uint32_t MaxExportedFlows = 30;

		if (_toExport.size() == 0) {
			return;
		}

		std::uint32_t nExports = _toExport.size();
		std::vector<std::uint8_t> data;
		std::uint32_t exportIndex = 0;

		// musíme exportovat maximálně po MaxExportedFlows záznamech
		while (nExports > 0) {
			const std::uint32_t currentExports = std::min(MaxExportedFlows, nExports);

			// _nFlowsSeen musíme přidávat postupně. Pokud je přidáme všechny najednou,
			// tak při exportu > MaxExportedFlows bude mít h.count nesprávné hodnoty
			_nFlowsSeen += currentExports;
			nExports -= currentExports;

			std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds> 
				tp{std::chrono::seconds{_currentTime.tv_sec}};

			// exportujeme maximálně MaxExportedFlows; zbytek v dalších exportech
			NetflowV5Header h {};
			h.count = currentExports;
			h.sysUptime = static_cast<uint32_t>((TimevalToSec(_currentTime) - TimevalToSec(_startTs)) * 1000.0);
			h.unixSecs = _currentTime.tv_sec;
			std::cout << _currentTime.tv_sec << " " << h.unixSecs << "\n";
			//h.unixSecs = tp.time_since_epoch().count();
			h.unixNsecs = _currentTime.tv_usec;
			h.flowSequence = _nFlowsSeen;

			auto hdrSerialized = h.Serialize();
			data.insert(data.end(), &hdrSerialized[0], &hdrSerialized[hdrSerialized.size()]);

			for (std::uint32_t i = 0; i < currentExports; i++) {
				auto serialized = _toExport.at(exportIndex++).Serialize();
				data.insert(data.end(), &serialized[0], &serialized[serialized.size()]);
			}

			// zašleme na kolektor
			const bool sent = _collector.Send(data.data(), data.size());
			if (!sent) {
				Logger::LogDebug<>("Couldn't send data");
			}
			data.clear();
		}
		_toExport.clear();
	}

	std::uint32_t NetflowExporter::CalculateIpChecksum(const Ipv4Header * iph)
	{
		constexpr std::uint16_t IpHeaderSize = 10;

		std::uint32_t total = 0;
		auto iphCast = reinterpret_cast<const uint16_t *>(iph);
		for (std::uint16_t i = 0; i < IpHeaderSize; i++) {
			total += iphCast[i];
		}
		return (total & 0x0000FFFF) + (total >> 16);
	}

	double NetflowExporter::TimevalToSec(const timeval & t)
	{
		return (static_cast<double>(t.tv_sec)) + (static_cast<double>(t.tv_usec) / 1'000'000);
	}
} // namespace Netflow
