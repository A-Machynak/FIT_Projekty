#include "models_wrap/langton_ant_wrapper.h"
#include "models_wrap/elementary_ca_wrapper.h"

#include "gui/gui.h"
#include "cli.h"

//constexpr CA::Coordinate2D InitialPosition { 200, 150 };
//constexpr CA::Direction2D InitialDirection = CA::Direction2D::Right;

ModelWrapper::ElementaryWrapper ParseCAArgs(const Cli::CliArgs & cli)
{
	const auto dim = CA::Dimension2D{static_cast<std::uint32_t>(cli.PlaneWidth), static_cast<std::uint32_t>(cli.PlaneHeight)};
	if (cli.CAArgs.size() != 2 && cli.CAArgs.size() != 3) {
		std::cerr << "-caargs - expected 2 or 3 arguments, received " << cli.CAArgs.size() << "\n";
		exit(3);
	}
	const std::string & ruleStrategy = cli.CAArgs[0];
	const std::string & initialState = cli.CAArgs[1];
	const std::string rules = cli.CAArgs.size() == 3 ? cli.CAArgs[2] : "rng";

	CA::RuleStrategy rs;
	if (ruleStrategy == "random") {
		rs = CA::RuleStrategy::Random;
	}
	else if (ruleStrategy == "drandom") {
		rs = CA::RuleStrategy::DatasetRandom;
	}
	else if (ruleStrategy == "dring") {
		rs = CA::RuleStrategy::DatasetRing;
	}
	else if (ruleStrategy == "diter") {
		rs = CA::RuleStrategy::DatasetIterate;
	}
	else {
		std::cerr << "-caargs - expected rule strategy (\"random\"/\"drandom\"/\"dring\"/\"diter\"), received " << ruleStrategy << "\n";
		exit(4);
	}

	std::vector<std::uint8_t> rulesVec;
	if (rules != "rng") {
		rulesVec = Cli::ParseArray(rules);
	}

	if (initialState == "rng") {
		ModelWrapper::ElementaryWrapper wrp(dim, rs, rulesVec, { });
		wrp.GetModel().SetInitialStateRandom();
		return wrp;
	}
	else if (initialState == "rng1") {
		ModelWrapper::ElementaryWrapper wrp(dim, rs, rulesVec, { });
		wrp.GetModel().SetInitialStateSingleRandom();
		return wrp;
	}
	else if (initialState == "mid") {
		ModelWrapper::ElementaryWrapper wrp(dim, rs, rulesVec, { });
		wrp.GetModel().SetInitialStateSingle(cli.PlaneWidth/2, true);
		return wrp;
	}
	//else {
	const auto arr = Cli::ParseArray(initialState);
	return ModelWrapper::ElementaryWrapper(dim, rs, rulesVec, arr);
}

int main(int argc, char **argv)
{
	std::string windowName = "Cellular Automaton";
	const Cli::CliArgs cli = Cli::ParseCli(argc, argv);

	if (cli.ErrorFlag != 0) {
		return (cli.ErrorFlag == 1) ? 0 : cli.ErrorFlag;
	}

	if (cli.CA != "eca") {
		std::cerr << "Unknown CA (" << cli.CA << ")\n";
		return 2;
	}

	auto model = ParseCAArgs(cli);

	Gui::PixelWindow pw(cli.WindowWidth, cli.WindowHeight, windowName,
		cli.PlaneWidth, cli.PlaneHeight, cli.OpenglOff);
	pw.SetTimestep((cli.TargetFps <= 0.0) ? (1.0 / 1000.0) : (1.0 / cli.TargetFps));
	pw.SetOutputFile(cli.OutputFile);

	bool ok = pw.Run(&model);
	
	return 0;
}
