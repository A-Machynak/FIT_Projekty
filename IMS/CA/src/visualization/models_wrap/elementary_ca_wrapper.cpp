#include "elementary_ca_wrapper.h"

#include <iostream>

namespace ModelWrapper
{
	ElementaryWrapper::ElementaryWrapper(
			CA::Dimension2D planeSize,
			CA::RuleStrategy strategy,
			const std::vector<CA::Rule> & rules,
			const std::vector<CA::Cell> & initialState)
		: _model(planeSize, strategy, rules)
	{
		_model.SetInitialState(initialState);
	}
	
	void ElementaryWrapper::Init(WindowData & windowData)
	{
		// nastavíme na 255 (bílá)
		std::fill(
			windowData.Bytes,
			windowData.Bytes + (windowData.Width*windowData.Height*3),
			255);
		
		// 1. generace
		const auto & board = _model.GetBoardState();
		const auto & row = board.GetRow(_model.GetRowIdx());
		const auto rowIdx = board.GetHeight() - 1;
		for (std::uint32_t i = 0; i < row.size(); i++) {
			const auto color = row[i] == 1 ? RgbBlack : RgbWhite;
			windowData.SetPixelUnsafe(i, rowIdx, color);
		}
	}

	void ElementaryWrapper::Step(WindowData & windowData)
	{
		const auto rule = _model.GetCurrentRule();
		_model.Step();
		_lastRowIdx = _model.GetRowIdx();

		const auto & board = _model.GetBoardState();
		const auto & row = board.GetRow(_lastRowIdx);
		const auto ruleColor = Gui::Colors[rule];
		//const auto ruleColor = Gui::RgbBlack;
		for (std::uint32_t i = 0; i < row.size(); i++) {
			const auto color = row[i] == 1 ? ruleColor : RgbWhite;
			windowData.SetPixelUnsafe(i, _lastRowIdx, color);
		}
	}

	bool ElementaryWrapper::EndCondition(WindowData & windowData)
	{
		return _lastRowIdx == 0;
	}

	void ElementaryWrapper::Reset(WindowData & windowData)
	{
		_lastRowIdx = 0;
		_model.Reset();

		// Reset 1. generace a nastavení náhodné buňky do 1
		_model.ResetInitialState();
		_model.SetInitialStateSingleRandom();

		std::cout << "[ElementaryWrapper]: Reset\n";
	}

	CA::Elementary & ElementaryWrapper::GetModel()
	{
		return _model;
	}
} // namespace ModelWrapper