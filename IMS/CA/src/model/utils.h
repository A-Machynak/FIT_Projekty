
#pragma once

#include <cstdint>
#include <vector>

namespace CA
{
	/**
	 * @brief Směr ve 2D
	 */
	enum class Direction2D
	{
		Up,
		Down,
		Left,
		Right
	};

	/**
	 * @brief Souřadnice ve 2D
	 */
	struct Coordinate2D
	{
		std::uint32_t X;
		std::uint32_t Y;
	};

	/**
	 * @brief Velikost ve 2D
	 */
	struct Dimension2D
	{
		std::uint32_t Width;
		std::uint32_t Height;
	};

	/**
	 * @brief Obecná třída pro manipulaci s 2D polem ("hrací plocha")
	 * 
	 * @tparam T typ uložený v poli
	 */
	template <typename T>
	class Board
	{
	public:
		/**
		 * @brief Konstruktor
		 * 
		 * @param width šířka
		 * @param height výška
		 */
		Board(Dimension2D size);

		/**
		 * @brief Getter prvku na zadané pozici
		 * 
		 * @param w šířka (x)
		 * @param h výška (y)
		 * @return T& prvek
		 */
		T & Get(Coordinate2D coord);

		/**
		 * @brief Const getter prvku na zadané pozici
		 * 
		 * @param w šířka (x)
		 * @param h výška (y)
		 * @return const T& prvek
		 */
		const T & Get(Coordinate2D coord) const;

		/**
		 * @brief Getter řádku
		 * 
		 * @param h výška (y)
		 * @return T& prvek
		 */
		std::vector<T> & GetRow(std::uint32_t h);

		/**
		 * @brief Const getter řádku
		 * 
		 * @param h výška (y)
		 * @return const T& prvek
		 */
		const std::vector<T> & GetRow(std::uint32_t h) const;

		/**
		 * @brief Getter prvku na zadané pozici s kontrolou bounds
		 * 
		 * @param w šířka (x)
		 * @param h výška (y)
		 * @return T& prvek
		 */
		T & At(Coordinate2D coord);

		/**
		 * @brief Const getter prvku na zadané pozici s kontrolou bounds
		 * 
		 * @param w šířka (x)
		 * @param h výška (y)
		 * @return const T& prvek
		 */
		const T & At(Coordinate2D coord) const;

		/**
		 * @brief Getter řádku s kontrolou bounds
		 * 
		 * @param h výška (y)
		 * @return T& prvek
		 */
		std::vector<T> & AtRow(std::uint32_t h);

		/**
		 * @brief Const getter řádku s kontrolou bounds
		 * 
		 * @param h výška (y)
		 * @return const T& prvek
		 */
		const std::vector<T> & AtRow(std::uint32_t h) const;

		/**
		 * @brief Getter šířky
		 * 
		 * @return std::uint32_t šířka
		 */
		std::uint32_t GetWidth() const;

		/**
		 * @brief Getter výšky
		 * 
		 * @return std::uint32_t výška
		 */
		std::uint32_t GetHeight() const;
	private:
		/// @brief Plocha - dvojitý vektor pro jednodušší přístup k řádku
		std::vector<std::vector<T>> _board;

		/// @brief Šířka
		const Dimension2D _size;
	};
} // namespace CA
