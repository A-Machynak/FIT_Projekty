
#pragma once

#include "utils.hpp"

namespace CA
{
	template <typename T>
	class IModel
	{
	public:
		IModel(Dimension2D planeSize) : _board(planeSize) {};

		virtual void Step() = 0;

		virtual const Board<T> & GetBoardState() const = 0;
	protected:
		/// @brief Plocha
		Board<T> _board;
	};
} // namespace CA;
