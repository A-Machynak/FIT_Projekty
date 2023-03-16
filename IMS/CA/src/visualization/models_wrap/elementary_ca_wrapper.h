#include <elementary.h>
#include <utils.h>

#include "../gui/stepper_i.h"

using namespace Gui;

namespace ModelWrapper
{
	class ElementaryWrapper : public IStepper
	{
	public:
		ElementaryWrapper(
			CA::Dimension2D planeSize,
			CA::RuleStrategy strategy,
			const std::vector<CA::Rule> & rules,
			const std::vector<CA::Cell> & initialState);

		void Init(WindowData & windowData) override;

		void Step(WindowData & windowData) override;

		bool EndCondition(WindowData & windowData) override;

		void Reset(WindowData & windowData) override;

		CA::Elementary & GetModel();
	private:
		CA::Elementary _model;

		std::uint32_t _lastRowIdx = 0;
	};
} // namespace ModelWrapper
