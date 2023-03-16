
#include "langton_ant_model.h"

namespace CA
{
	LangtonAntModel::LangtonAntModel(Dimension2D planeSize, Coordinate2D initialPosition, Direction2D initialDirection)
		: _planeSize(planeSize), _currentPosition(initialPosition), _currentDirection(initialDirection)
	{
		_plane.resize(_planeSize.Width * _planeSize.Height);

		if (initialPosition.X < 0 || initialPosition.X >= planeSize.Width || initialPosition.Y < 0 || initialPosition.Y >= planeSize.Height) {
			_currentPosition.X = _planeSize.Width / 2;
			_currentPosition.Y = _planeSize.Height / 2;
		}
	}

	bool LangtonAntModel::GetStateAt(std::uint32_t x, std::uint32_t y) const
	{
		if (x < 0 || x >= _planeSize.Width || y < 0 || y >= _planeSize.Height) {
			return false;
		}

		return _plane[x + y * _planeSize.Width];
	}

	void LangtonAntModel::Step()
	{
		const auto pos = _currentPosition.X + _currentPosition.Y * _planeSize.Width;
		const bool currentSquare = _plane.at(pos);

		if (currentSquare == 1) { // černé políčko
			// otočit o 90 stupňů proti směru hodinových ručiček
			TurnCounterClockwise();
		}
		else { // bílé políčko
			// otočit o 90 stupňů po směru hodinových ručiček
			TurnClockwise();
		}
		// změnit barvu políčka
		FlipSquareColor();
		// pohyb o 1 políčko dopředu
		MoveForward();
	}

	const std::vector<std::uint8_t> & LangtonAntModel::GetPlaneState() const
	{
		return _plane;
	}

	const Coordinate2D & LangtonAntModel::GetCurrentPosition() const
	{
		return _currentPosition;
	}

	void LangtonAntModel::TurnClockwise()
	{
		switch(_currentDirection) {
		case Direction2D::Up:
			_currentDirection = Direction2D::Right;
			break;
		case Direction2D::Down:
			_currentDirection = Direction2D::Left;
			break;
		case Direction2D::Left:
			_currentDirection = Direction2D::Up;
			break;
		case Direction2D::Right:
			_currentDirection = Direction2D::Down;
			break;
		}
	}
	
	void LangtonAntModel::TurnCounterClockwise()
	{
		switch(_currentDirection) {
		case Direction2D::Up:
			_currentDirection = Direction2D::Left;
			break;
		case Direction2D::Down:
			_currentDirection = Direction2D::Right;
			break;
		case Direction2D::Left:
			_currentDirection = Direction2D::Down;
			break;
		case Direction2D::Right:
			_currentDirection = Direction2D::Up;
			break;
		}
	}

	void LangtonAntModel::MoveForward()
	{
		// Pohybujeme se dopředu pouze v případě, že nevyjdeme z plochy
		switch (_currentDirection) {
		case Direction2D::Up:
			if (_currentPosition.Y != 0) {
				_currentPosition.Y -= 1;
			}
			break;
		case Direction2D::Down:
			if (_currentPosition.Y != _planeSize.Height-1) {
				_currentPosition.Y += 1;
			}
			break;
		case Direction2D::Left:
			if (_currentPosition.X != 0) {
				_currentPosition.X -= 1;
			}
			break;
		case Direction2D::Right:
			if (_currentPosition.X != _planeSize.Width-1) {
				_currentPosition.X += 1;
			}
			break;
		}
	}

	void LangtonAntModel::FlipSquareColor()
	{
		const auto pos = _currentPosition.X + _currentPosition.Y * _planeSize.Width;
		auto & value = _plane.at(pos);
		value = !value;
	}
}