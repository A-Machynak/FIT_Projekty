/**
 * @file netflow_collector_connection.cpp
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Implementace
 */

#include "netflow_collector_connection.h"

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

//#define _loggerDebug
#include "logger/logger.hpp"

namespace Netflow
{
	CollectorConnection::CollectorConnection(const std::string & collectorIp, std::uint32_t port)
	{
		Logger::LogInfo<>("Initializing UDP socket: " + collectorIp + ":" + std::to_string(port));
		_initialized = InitConnection(collectorIp.c_str(), std::to_string(port).c_str());
		if (!_initialized) {
			Logger::LogError<>("Couldn't initialize socket");
		}
	}

	CollectorConnection::~CollectorConnection()
	{
		close(_sockfd);
	}

	bool CollectorConnection::IsInitialized() const
	{
		return _initialized;
	}

	bool CollectorConnection::InitConnection(const char * collectorIp, const char * collectorPort)
	{
		sockaddr_in serverAddress {};
		addrinfo hints {}, *result;
		int err;

		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_DGRAM;

		err = getaddrinfo(collectorIp, collectorPort, &hints, &result);
		if (err != 0) {
			Logger::LogError<>("getaddrinfo failed: " + std::string(gai_strerror(err)));
			return false;
		}

		for (addrinfo *p = result; p != nullptr; p = p->ai_next) {
			_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
			if (_sockfd != -1) {
				_collectorAddr = *p->ai_addr;
				_collectorAddrLen = p->ai_addrlen;

				Logger::LogInfo<>("Created socket.");
				break;
			}
		}
		
		freeaddrinfo(result);
		return _sockfd != -1;
	}

	bool CollectorConnection::Send(const std::uint8_t * data, int len) const
	{
		if (!_initialized) {
			return false;
		}

		int sent = sendto(_sockfd, data, len, 0, &_collectorAddr, _collectorAddrLen);
		Logger::LogDebug<>("Sent data");
		if (sent == -1) {
			Logger::LogWarning<>("Failed sending " + std::to_string(len) + " bytes of data.");
		}

		return sent != -1;
	}
} // namespace Netflow
