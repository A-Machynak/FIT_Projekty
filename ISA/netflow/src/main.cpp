/**
 * @file main.cpp
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz)
 * @brief Vstupní bod programu
 */

#include <iostream>

//#define _loggerDebug
#include "project/logger/logger.hpp"

#include "project/netflow_exporter.h"

/// @brief Výchozí hodnota pro jméno analyzovaného souboru ("-" značí STDIN)
const std::string DefaultFile = "-";

/// @brief Výchozí hodnota pro ip adresu netflow kolektoru
const std::string DefaultCollectorIp = "127.0.0.1";

/// @brief Výchozí hodnota pro port netflow kolektoru
constexpr std::uint32_t DefaultCollectorPort = 2055;

/// @brief Výchozí hodnota pro export aktivních záznamů
constexpr std::uint32_t DefaultActiveTimer = 60;

/// @brief Výchozí hodnota pro interval exportu neaktivních záznamů
constexpr std::uint32_t DefaultInterval = 10;

/// @brief Výchozí hodnota pro velikost flow-cache
constexpr std::uint32_t DefaultFlowCacheSize = 1024;

struct CliInput
{
	std::string file {DefaultFile};
	std::string collectorIp {DefaultCollectorIp};
	std::uint32_t collectorPort {DefaultCollectorPort};
	std::uint32_t activeTimer {DefaultActiveTimer};
	std::uint32_t interval {DefaultInterval};
	std::uint32_t flowCacheSize {DefaultFlowCacheSize};
	int errorFlag {0};
};

/**
 * @brief Vypíše použití na std::cout
 */
void PrintUsage()
{
	std::cout << "flow\nUsage:\n\t."
		<< "/flow [-f <file>] [-c <netflow_collector>[:<port>]] [-a <active_timer>] [-i <inactive_timer>] [-m <count>]\n"
		<< "Flags:\n"
		<< "\t-f\t - File to analyze (default: STDIN)\n"
		<< "\t-c\t - IP address/hostname of netflow collector (default: 127.0.0.1:2055)\n"
		<< "\t-a\t - Netflow active timer (default: 60)\n"
		<< "\t-i\t - Netflow inactive timer (default: 10)\n"
		<< "\t-m\t - Netflow flow-cache size (default: 1024)\n";
}

/**
 * @brief Parsování argumentů z cli
 * 
 * @param argc počet argumentů
 * @param argv argumenty
 * @return CliInput naparsované argumenty
 */
CliInput ParseCli(int argc, char **argv)
{
	CliInput in;

	enum class Expect
	{
		Flag,
		File,
		Collector,
		ActiveTimer,
		Interval,
		FlowCacheSize
	};
	Expect ex = Expect::Flag;

	for (int i = 1; i < argc; i++) {
		std::string arg(argv[i]);

		switch (ex) {
		case Expect::Flag:
			if (arg == "-f") {
				ex = Expect::File;
			}
			else if (arg == "-c") {
				ex = Expect::Collector;
			}
			else if (arg == "-a") {
				ex = Expect::ActiveTimer;
			}
			else if (arg == "-i") {
				ex = Expect::Interval;
			}
			else if (arg == "-m") {
				ex = Expect::FlowCacheSize;
			}
			else if (arg == "-h") {
				PrintUsage();
				in.errorFlag = -1;
			}
			else {
				Logger::LogError<>("Expected flag (-f | -c | -a | -i | -m): " + arg);
				in.errorFlag = 2;
			}
			break;
		case Expect::File:
			in.file = arg;
			ex = Expect::Flag;
			break;
		case Expect::Collector: {
			const auto colonPos = arg.find(":");
			if (colonPos != std::string::npos) {
				in.collectorIp = arg.substr(0, colonPos);
				in.collectorPort = std::stoul(arg.substr(colonPos+1, arg.size()));
			}
			else {
				in.collectorIp = arg;
			}
			ex = Expect::Flag;
			break;
		}
		case Expect::ActiveTimer:
			in.activeTimer = std::stoul(arg);
			ex = Expect::Flag;
			break;
		case Expect::FlowCacheSize:
			in.flowCacheSize = std::stoul(arg);
			ex = Expect::Flag;
			break;
		default:
			Logger::LogDebug<>("CLI - unexpected state");
			ex = Expect::Flag; // nemělo by dojít, dead code
			break;
		}

		if (in.errorFlag != 0) {
			break;
		}
	}

	return in;
}

int main(int argc, char **argv)
{
	CliInput cli = ParseCli(argc, argv);
	if (cli.errorFlag != 0) {
		return cli.errorFlag == -1 ? 0 : cli.errorFlag;
	}

	auto n = Netflow::NetflowExporter(cli.file, cli.collectorIp, cli.collectorPort, cli.activeTimer, cli.interval, cli.flowCacheSize);

	Logger::LogInfo<>("Starting netflow exporter...");
	n.Run();

	return 0;
}
