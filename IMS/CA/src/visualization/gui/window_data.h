
#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "rgb.h"

namespace Gui
{
	/**
	 * @brief Pomocná třída pro manipulaci s pixely pro uživatele
	 */
	struct WindowData
	{
		const int Width;  ///< Šířka okna
		const int Height; ///< Výška okna
		GLubyte * Bytes;  ///< Pole bytů pro pixely (velikost = 3 * Width * Height)

		/**
		 * @brief Konstruktor
		 * 
		 * @param width šířka okna
		 * @param height výška okna
		 */
		WindowData(int width, int height);

		/**
		 * @brief Destruktor (dealokace Bytes)
		 */
		~WindowData();

		/**
		 * @brief Nastaví barvu pixelu na zadaných souřadnicích
		 * 
		 * @param x souřadnice
		 * @param y souřadnice
		 * @param color barva
		 * @return true barva byla nastavena
		 * @return false zadány souřadnice mimo hranice
		 */
		bool SetPixel(int x, int y, RGB color);

		/**
		 * @brief Nastaví barvu pixelu na zadaných souřadnicích (předpoklad správně zadaných souřadnic).
		 * Neprovádí kontrolu hodnot x a y souřadnic - UB pokud jsou zadány hodnoty mimo meze.
		 * 
		 * @param x souřadnice
		 * @param y souřadnice
		 * @param color barva
		 */
		void SetPixelUnsafe(int x, int y, RGB color);

		/**
		 * @brief Getter barvy pixelu
		 * 
		 * @param x souřadnice
		 * @param y souřadnice
		 * @param err (volitelné) nastaven na true v případě, že je zadána nevalidní souřadnice
		 * @return RGB barva
		 */
		RGB GetPixel(int x, int y, bool * err = nullptr) const;

		/**
		 * @brief Getter barvy pixelu (předpoklad správně zadaných souřadnic).
		 * Neprovádí kontrolu hodnot x a y souřadnic - UB pokud jsou zadány hodnoty mimo meze.
		 * 
		 * @param x souřadnice
		 * @param y souřadnice
		 * @return RGB barva
		 */
		RGB GetPixelUnsafe(int x, int y) const;
	};
} // namespace Gui
