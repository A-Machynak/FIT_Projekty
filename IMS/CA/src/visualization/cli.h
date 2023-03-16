
#include <string>
#include <vector>

namespace Cli
{
	namespace Default
	{
		constexpr double TargetFps {240.0};
		constexpr int WindowWidth {800};
		constexpr int WindowHeight {800};
		constexpr int PlaneWidth {800};
		constexpr int PlaneHeight {800};
	};

	struct CliArgs
	{
		int ErrorFlag {0}; // 1 = bez erroru, ale je vyžadován exit() (-h flag)
		double TargetFps {Default::TargetFps};
		int WindowWidth {Default::WindowWidth};
		int WindowHeight {Default::WindowHeight};
		int PlaneWidth {Default::PlaneWidth};
		int PlaneHeight {Default::PlaneHeight};
		bool OpenglOff {false};
		std::string OutputFile {"CA_Out"};
		std::string CA {"eca"};
		std::vector<std::string> CAArgs {"diter", "rng1", "30"};
	};

	CliArgs ParseCli(int argc, char **argv);

	std::vector<std::uint8_t> ParseArray(const std::string & strArr);
} // namespace Cli
