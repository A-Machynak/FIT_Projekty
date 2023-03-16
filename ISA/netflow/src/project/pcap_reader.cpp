/**
 * @file pcap_reader.cpp
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Implementace
 */

#include "pcap_reader.h"

#include "logger/logger.hpp"

namespace Netflow
{
	Reader::Reader(const std::string & file) : _file(file)
	{
		OpenPcapFile();
	}

	void Reader::OpenPcapFile()
	{
		char errbuf[PCAP_ERRBUF_SIZE];

		_pcap = pcap_open_offline(_file.c_str(), errbuf);
		if (_pcap == nullptr) {
			if (_file == "-") {
				Logger::LogError<>("Couldn't read from stdin stream:");
			}
			else {
				Logger::LogError<>("Couldn't open " + _file + ":");
			}
			std::cerr << errbuf << "\n";
		}
	}

	std::uint64_t Reader::GetPacketCount()
	{
		return _packetCount;
	}

	Packet Reader::GetNextPacket()
	{
		Packet pkt;
		if (_pcap == nullptr) {
			return pkt;
		}
		int err = pcap_next_ex(_pcap, &pkt.pktHeader, &pkt.pktData);

		if (err == 1) {
			_packetCount++;
		}
		else {
			pkt.pktData = nullptr;
			pkt.pktHeader = nullptr;
		}
		return pkt;
	}

} // namespace Netflow

