#include "cli.h"

#include <unordered_map>
#include <iostream>

namespace Cli
{
	void PrintHelp()
	{
		std::cout << "Cellular automaton visualization\n"
		"Usage:\n"
		"\tCA_Visualization [-h] [flags]\n"
		"Flags:\n"
		"\t-f FRAMERATE \t - simulation speed in frames per second (default: 240)\n"
		"\t-ww WINDOW_WIDTH \t - window width (default: 800)\n"
		"\t-wh WINDOW_HEIGHT \t - window height (default: 800)\n"
		"\t-pw PLANE_WIDTH \t - CA plane width (default: 100)\n"
		"\t-ph PLANE_HEIGHT \t - CA plane height (default: 100)\n"
		"\t-out FILE_NAME \t - File name to save an image (default: CA_Out)\n"
		"\t-gloff\t - Turns off opengl - final state will be written to a file (default: on)\n"
		"\t-ca CA_STRING \t - string represenstation of a cellular automaton (currently only \"eca\" is a valid CA)\n"
		"\t-caargs CA_ARGS \t - CA arguments (always the last flag)\n\n"
		"CA Arguments:\n"
		" Arguments for \"eca\":\n"
		" Arguments are entered in the following format:\n"
		"\t {RULE_STRATEGY} {INITIAL_STATE} {RULES}\n"
		"\t All the values are separated by a comma (,) and value sets are separated by spaces\n\n"
		"\t Rules are values between 0 and 255.\n\n"
		"\t Initial states are values between 0 and 1 and represent the initial states (on/off) of the first generation.\n"
		"\t  Maximum of PLANE_WIDTH values can be entered. If less than PLANE_WIDTH values are entered,\n"
		"\t  the rest of the values are set by repeating the entered values (0,1 -> 0,1,0,1,0, ...)\n\n"
		"\t Rule strategy is one of the following:\n"
		"\t  - \"random\" - each generation a random rule is applied, {RULES} are ignored in this case\n"
		"\t  - \"drandom\" - each generation a random rule is applied from the {RULES} set\n"
		"\t  - \"dring\" - the rule sequence is cyclically repeating (ring buffer)\n"
		"\t  - \"diter\" - same as the \"dring\", but instead of cyclically repeating the last rule is applied on all the remaining generations\n\n"
		"\n"
		"\tSpecial keywords:\n"
		"\t \"rng\" in {INITIAL_STATE} - initial state is chosen randomly\n"
		"\t \"rng1\" in {INITIAL_STATE} - initial state is chosen randomly by setting a single random cell to 1\n"
		"\t \"mid\" in {INITIAL_STATE} - initial state is chosen by setting a single cell in the middle to 1\n"
		"\n"
		"Controls:\n"
		"\tLeft click and drag - moves the viewport\n"
		"\tRight click - resets the viewport\n"
		"\tMouse scroll - zooms the viewport\n"
		"\tR - resets the CA\n"
		"\tP - saves an image to a file specified by \"-out\" flag\n"
		"\n"
		"Examples:\n"
		"\tRandom rule strategy, initial state {0, 1, 0, 1, ...}, rule 30 (ignored)\n"
		"\tCA_Visualization -f 30 -ca eca -caargs random 0,1 30\n"
		"\n"
		"\tRandom rule strategy, random initial state, rule 30 (ignored)\n"
		"\tCA_Visualization -f 30 -ca eca -caargs random rng 30\n"
		"\n"
		"\tCyclical rule strategy, random cell set to 1, rule 30 and 60 (applied as 30, 60, 30, 60, ...)\n"
		"\tCA_Visualization -f 30 -ca eca -caargs dring rng1 30,60\n"
		"\n"
		"\tRegular Rule 30 elementary CA with random cell set to 1\n"
		"\tCA_Visualization -f 30 -ca eca -caargs diter rng1 30\n"
		"\n"
		"\tCyclical rule strategy, middle cell set to 1, rule 105 and 106, opengl off (result saved to CA_Out.png)\n"
		"\tCA_Visualization -gloff -caargs dring mid 105,106\n";
	}

	CliArgs ParseCli(int argc, char **argv)
	{
		CliArgs cliArgs;
		enum class Expect
		{
			Flag,
			Frames,
			WindowWidth,
			WindowHeight,
			PlaneWidth,
			PlaneHeight,
			CA,
			CAArgs,
			OutFile,
			OpenglOff,
		};

		static std::unordered_map<std::string, Expect> map {
			{ "-f", Expect::Frames },
			{ "-ww", Expect::WindowWidth },
			{ "-wh", Expect::WindowHeight },
			{ "-pw", Expect::PlaneWidth },
			{ "-ph", Expect::PlaneHeight },
			{ "-ca", Expect::CA },
			{ "-caargs", Expect::CAArgs },
			{ "-out", Expect::OutFile },
		};

		Expect e = Expect::Flag;

		for (int i = 1; i < argc; i++) {
			const std::string arg { argv[i] };

			switch (e) {
			case Expect::Flag: {
				if (arg == "-h" || arg == "--help") {
					PrintHelp();
					cliArgs.ErrorFlag = 1;
					return cliArgs;
				}
				else if (arg == "-gloff") {
					cliArgs.OpenglOff = true;
					break;
				}

				auto found = map.find(arg);
				if (found == map.end()) {
					cliArgs.ErrorFlag = 2;
					std::cerr << "Unknown flag \"" << arg << "\"\n";
					PrintHelp();
					return cliArgs;
				}
				e = found->second;
				break;
			}
			case Expect::Frames:
				cliArgs.TargetFps = std::stod(arg);
				e = Expect::Flag;
				break;
			case Expect::WindowWidth:
				cliArgs.WindowWidth = std::stoi(arg);
				e = Expect::Flag;
				break;
			case Expect::WindowHeight:
				cliArgs.WindowHeight = std::stoi(arg);
				e = Expect::Flag;
				break;
			case Expect::PlaneWidth:
				cliArgs.PlaneWidth = std::stoi(arg);
				e = Expect::Flag;
				break;
			case Expect::PlaneHeight:
				cliArgs.PlaneHeight = std::stoi(arg);
				e = Expect::Flag;
				break;
			case Expect::CA:
				cliArgs.CA = arg;
				e = Expect::Flag;
				break;
			case Expect::CAArgs:
				cliArgs.CAArgs.clear();
				for (int j = i; j < argc; j++) {
					cliArgs.CAArgs.push_back(std::string(argv[j]));
				}
				i = argc; // konec
				e = Expect::Flag;
				break;
			case Expect::OutFile:
				cliArgs.OutputFile = arg;
				e = Expect::Flag;
				break;
			}
		}

		return cliArgs;
	}

	std::vector<std::uint8_t> ParseArray(const std::string & strArr)
	{
		std::vector<std::uint8_t> vec;
		std::string str = "";
		for (std::uint32_t i = 0; i < strArr.size(); i++) {
			if (strArr[i] == ',') {
				vec.push_back(std::stoi(str));
				str = "";
				continue;
			}
			str += strArr[i];
		}
		vec.push_back(std::stoi(str));
		return vec;
	}
} // namespace Cli