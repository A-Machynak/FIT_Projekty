
#pragma once

#include <vector>
#include <cstdint>
#include <iostream>
#include <vector>

#include "utils.h"

namespace CA
{
	/**
	 * @brief Model Langtonova mravence.
	 * TODO: Potřebuje refraktorizaci - úprava na rozšíření IModel.
	 * TODO: Lze zobecnit (turmite)
	 * 
	 * Pravidla:
	 * - Pokud je mravenec na bílém čtvreci, otočí se o 90 stupňů po směru hodinových ručiček,
	 *   změní barvu čtverce a pohne se o 1 čtverec dopředu
	 * - Pokud je mravenec na černém čtvreci, otočí se o 90 stupňů proti směru hodinových ručiček,
	 *   změní barvu čtverce a pohne se o 1 čtverec dopředu
	 */
	class LangtonAntModel
	{
	public:
		/**
		 * @brief Konstruktor
		 * 
		 * @param planeSize velikost plochy, po které se mravenec pohybuje
		 * @param initialPosition prvotní pozice mravence (pokud je mimo meze, tak je mravenec umístěn doprostřed plochy)
		 * @param initialDirection prvotní směr mravence
		 */
		LangtonAntModel(Dimension2D planeSize, Coordinate2D initialPosition, Direction2D initialDirection);

		/**
		 * @brief Provede 1 krok mravence
		 */
		void Step();

		/**
		 * @brief Getter stavu buňky na daných souřadnicích
		 * 
		 * @param x souřadnice
		 * @param y souřadnice
		 * @return true buňka je černá
		 * @return false buňka je bílá
		 */
		bool GetStateAt(std::uint32_t x, std::uint32_t y) const;

		/**
		 * @brief Getter stavu celé plochy
		 * 
		 * @return const std::vector<std::uint8_t>& stav plochy
		 */
		const std::vector<std::uint8_t> & GetPlaneState() const;

		/**
		 * @brief Getter současné pozice mravence
		 * 
		 * @return const Coordinate2D& pozice mravence
		 */
		const Coordinate2D & GetCurrentPosition() const;
	private:
		/// @brief Velikost plochy, po které se mravenec pohybuje
		const Dimension2D _planeSize;

		/// @brief Současná pozice mravence
		Coordinate2D _currentPosition;

		/// @brief Směr mravence
		Direction2D _currentDirection;

		/// @brief Současný stav plochy (true - černé políčko; false - bílé políčko)
		/// Note: katastrofu jménem std::vector<bool> odmítám používat
		std::vector<std::uint8_t> _plane;

		/**
		 * @brief Otočí mravence ve směru hodinových ručiček (`_currentDirection`)
		 */
		void TurnClockwise();

		/**
		 * @brief Otočí mravence proti směru hodinových ručiček (`_currentDirection`)
		 */
		void TurnCounterClockwise();

		/**
		 * @brief Posune mravence o jedno políčko dopředu podle současného směru, pokud je to možné (`_currentPosition`)
		 */
		void MoveForward();

		/**
		 * @brief Změní barvu políčka, na kterém mravenec stojí
		 */
		void FlipSquareColor();
	};
}
