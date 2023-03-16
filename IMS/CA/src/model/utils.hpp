#include "utils.h"

namespace CA
{
	template <typename T>
	Board<T>::Board(Dimension2D size)
		: _board(size.Height), _size(size)
	{
		for (std::uint32_t h = 0; h < size.Height; h++) {
			_board[h].resize(size.Width);
		}
	}

	template <typename T>
	T & Board<T>::Get(Coordinate2D coord)
	{
		return _board[coord.Y][coord.X];
	}

	template <typename T>
	const T & Board<T>::Get(Coordinate2D coord) const
	{
		return _board[coord.Y][coord.X];
	}

	template <typename T>
	std::vector<T> & Board<T>::GetRow(std::uint32_t h)
	{
		return _board[h];
	}

	template <typename T>
	const std::vector<T> & Board<T>::GetRow(std::uint32_t h) const
	{
		return _board[h];
	}

	template <typename T>
	T & Board<T>::At(Coordinate2D coord)
	{
		return _board.at(coord.Y).at(coord.X);
	}

	template <typename T>
	const T & Board<T>::At(Coordinate2D coord) const
	{
		return _board.at(coord.Y).at(coord.X);
	}

	template <typename T>
	std::vector<T> & Board<T>::AtRow(std::uint32_t h)
	{
		return _board.at(h);
	}

	template <typename T>
	const std::vector<T> & Board<T>::AtRow(std::uint32_t h) const
	{
		return _board.at(h);
	}

	template <typename T>
	std::uint32_t Board<T>::GetWidth() const
	{
		return _size.Width;
	}

	template <typename T>
	std::uint32_t Board<T>::GetHeight() const
	{
		return _size.Height;
	}
} // namespace CA
