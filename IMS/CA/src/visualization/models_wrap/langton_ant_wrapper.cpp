#include "langton_ant_wrapper.h"

namespace ModelWrapper
{
	LangtonsAntWrapper::LangtonsAntWrapper(
			CA::Dimension2D planeSize,
			CA::Coordinate2D initialPosition,
			CA::Direction2D initialDirection) :
		_model(planeSize, initialPosition, initialDirection)
	{
	}
	
	void LangtonsAntWrapper::Init(WindowData & windowData)
	{
		// nastavíme na 255 (bílá)
		std::fill(
			windowData.Bytes,
			windowData.Bytes + (windowData.Width*windowData.Height*3),
			255);
	}

	void LangtonsAntWrapper::Step(WindowData & windowData)
	{
		// využijeme faktu, že se mění barva pouze na staré pozici mravence
		const auto oldPos = _model.GetCurrentPosition();
		_model.Step();

		const auto color = _model.GetStateAt(oldPos.X, oldPos.Y)
			? RgbBlack
			: RgbWhite;
		windowData.SetPixelUnsafe(oldPos.X, oldPos.Y, color);
	}
} // namespace ModelWrapper