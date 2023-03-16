#include "elementary.h"

#include <algorithm>
#include <iostream>

namespace CA
{
	Elementary::Elementary(Dimension2D planeSize) : Elementary(planeSize, RuleStrategy::Random, {})
	{
	}

	Elementary::Elementary(Dimension2D planeSize, RuleStrategy strategy, const std::vector<Rule> & rules)
		: IModel<Cell>(planeSize), _strategy(rules.size() == 0 ? RuleStrategy::Random : strategy),
		_rules(rules)
	{
		ResetInitialState();
		InitRand();
		InitRandomRules();
	}

	void Elementary::InitRand()
	{
		std::srand(static_cast<unsigned int>(std::time(0)));
	}

	void Elementary::InitRandomRules()
	{
		if (_strategy == RuleStrategy::Random) {
			_rules.clear(); // pro jistotu odebereme vše

			const std::uint32_t ruleCount = _board.GetHeight();
			_rules.reserve(ruleCount);
			for (std::uint32_t i = 0; i < ruleCount; i++) {
				const auto rule = static_cast<Rule>(std::rand());
				_rules.push_back(rule);
			}
		}
		else if (_strategy == RuleStrategy::DatasetRandom) {
			// ať je i první index náhodný
			UpdateRuleIdxDatasetRandom();
		}
	}

	void Elementary::Reset()
	{
		_currentGen = 0;
		_currentRuleIdx = 0;
		InitRandomRules();
	}

	void Elementary::SetInitialState(const std::vector<Cell> & state)
	{
		if (state.size() == 0) {
			return;
		}
		std::uint32_t stateIdx = 0;
		auto & row = _board.GetRow(GetRowIdx());

		for (std::uint32_t i = 0; i < _board.GetWidth(); i++) {
			row[i] = state[stateIdx];
			stateIdx = (stateIdx == state.size() - 1) ? 0 : stateIdx + 1;
		}
	}

	void Elementary::SetInitialStateRandom()
	{
		auto & row = _board.GetRow(GetRowIdx());
		for (std::uint32_t i = 0; i < _board.GetWidth(); i++) {
			row[i] = static_cast<Cell>(std::rand() & 1);
		}
	}


	void Elementary::SetInitialStateSingle(std::uint32_t x, bool set)
	{
		if (x >= _board.GetWidth()) {
			return;
		}
		auto & row = _board.GetRow(GetRowIdx());
		row[x] = set ? 1 : 0;
	}

	void Elementary::SetInitialStateSingleRandom()
	{
		auto & row = _board.GetRow(GetRowIdx());
		const auto pos = static_cast<std::uint32_t>(std::rand()) % _board.GetWidth();
		row[pos] = 1;
	}

	void Elementary::ResetInitialState()
	{
		auto & row = _board.GetRow(GetRowIdx());
		std::fill(row.begin(), row.end(), 0);
	}

	void Elementary::Step()
	{
		const auto boardHeight = _board.GetHeight();
		if (_currentGen == boardHeight - 1) {
			return;
		}

		const auto & row = _board.GetRow(GetRowIdx());
		const auto & rule = _rules[_currentRuleIdx];
		auto & nextGenRow = _board.GetRow(GetRowIdx() - 1);
		//std::cout << "Applying rule " << static_cast<int>(rule) << "\n";
		for (std::uint32_t i = 0; i < _board.GetWidth(); i++) {
			const std::uint8_t neighbourhood = GetNeighbourhood(i, row);
			nextGenRow[i] = ((1 << neighbourhood) & rule) ? 1 : 0;
		}

		UpdateRuleIdxFromStrategy();
		_currentGen++;
	}

	std::uint8_t Elementary::GetNeighbourhood(std::uint32_t centerX, const std::vector<Cell> & cellRow)
	{
		const std::uint32_t leftIdx = (centerX == 0) ? (cellRow.size()-1) : (centerX - 1);
		const std::uint32_t rightIdx = (centerX == cellRow.size()-1) ? 0 : centerX + 1;

		return (cellRow[leftIdx] == 1) << 0
			 | (cellRow[centerX] == 1) << 1
			 | (cellRow[rightIdx] == 1) << 2;
	}

	void Elementary::UpdateRuleIdxFromStrategy()
	{
		switch(_strategy) {
			case RuleStrategy::DatasetIterate:
				UpdateRuleIdxDatasetIterate();
				break;
			case RuleStrategy::DatasetRandom:
				UpdateRuleIdxDatasetRandom();
				break;
			case RuleStrategy::DatasetRing:
				UpdateRuleIdxDatasetRing();
				break;
			default: // Random
				UpdateRuleIdxRandom();
				break;
		}
	}

	void Elementary::UpdateRuleIdxRandom()
	{
		// pravidla jsou již (pseudo)náhodně vygenerována, takže chování je identické
		UpdateRuleIdxDatasetIterate();
	}

	void Elementary::UpdateRuleIdxDatasetRandom()
	{
		const auto idx = static_cast<std::uint32_t>(std::rand());
		_currentRuleIdx = idx % _rules.size();
	}

	void Elementary::UpdateRuleIdxDatasetRing()
	{
		_currentRuleIdx = (_currentRuleIdx == _rules.size() - 1) ? 0 : _currentRuleIdx + 1;
	}

	void Elementary::UpdateRuleIdxDatasetIterate()
	{
		if (_currentRuleIdx != _rules.size() - 1) {
			_currentRuleIdx++;
		}
	}

	const Board<Cell> & Elementary::GetBoardState() const
	{
		return _board;
	}

	const std::vector<Rule> & Elementary::GetRules() const
	{
		return _rules;
	}

	Rule Elementary::GetCurrentRule() const
	{
		return _rules[_currentRuleIdx];
	}

	std::uint32_t Elementary::GetGeneration() const
	{
		return _currentGen;
	}

	std::uint32_t Elementary::GetRowIdx() const
	{
		return _board.GetHeight() - _currentGen - 1;
	}
} // namespace CA

