#include <langton_ant_model.h>
#include <utils.h>

#include "../gui/stepper_i.h"

using namespace Gui;

namespace ModelWrapper
{
	class LangtonsAntWrapper : public IStepper
	{
	public:
		LangtonsAntWrapper(
			CA::Dimension2D planeSize,
			CA::Coordinate2D initialPosition,
			CA::Direction2D initialDirection);

		void Init(WindowData & windowData) override;

		void Step(WindowData & windowData) override;

	private:
		CA::LangtonAntModel _model;
	};
} // namespace ModelWrapper
