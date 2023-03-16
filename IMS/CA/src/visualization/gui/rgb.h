
#pragma once

#include <cstdint>
#include <string>
#include <array>

namespace Gui
{
	/**
	 * @brief RGB hodnoty
	 */
	struct RGB
	{
		std::uint8_t R {};
		std::uint8_t G {};
		std::uint8_t B {};

		constexpr RGB(std::uint8_t r, std::uint8_t g, std::uint8_t b) : R(r), G(g), B(b) {}

		constexpr RGB(double redPercentage, double greenPercentage, double bluePercentage)
			: R(static_cast<std::uint8_t>(redPercentage / 100.0 * 255.0)),
			  G(static_cast<std::uint8_t>(greenPercentage / 100.0 * 255.0)),
			  B(static_cast<std::uint8_t>(bluePercentage / 100.0 * 255.0))
		{}
		
		/**
		 * @brief Operátor ekvivalence
		 * 
		 * @param other rgb
		 * @return true barvy jsou stejné
		 * @return false barvy nejsou stejné
		 */
		bool operator==(const RGB & other) const;

		/**
		 * @brief ToString metoda (pro debug)
		 * 
		 * @return std::string string reprezentace
		 */
		std::string ToString() const;
	};

	constexpr RGB RgbRed   = RGB(static_cast<std::uint8_t>(255), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0));
	constexpr RGB RgbGreen = RGB(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(255), static_cast<std::uint8_t>(0));
	constexpr RGB RgbBlue  = RGB(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(255));
	constexpr RGB RgbWhite = RGB(static_cast<std::uint8_t>(255), static_cast<std::uint8_t>(255), static_cast<std::uint8_t>(255));
	constexpr RGB RgbBlack = RGB(static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0), static_cast<std::uint8_t>(0));

	/// @brief "Náhodné" barvy, které jsou aspoň trochu odlišné
	constexpr std::array<RGB, 256> Colors = {
		RGB(0.00, 28.00, 73.00),
		RGB(69.00, 75.00, 10.00),
		RGB(49.00, 73.00, 91.00),
		RGB(70.00, 52.00, 75.00),
		RGB(45.00, 63.00, 76.00),
		RGB(94.00, 90.00, 100.00),
		RGB(86.00, 18.00, 26.00),
		RGB(77.00, 38.00, 6.00),
		RGB(94.00, 87.00, 80.00),
		RGB(62.00, 17.00, 41.00),
		RGB(95.00, 61.00, 73.00),
		RGB(67.00, 15.00, 31.00),
		RGB(23.00, 48.00, 34.00),
		RGB(100.00, 75.00, 0.00),
		RGB(60.00, 40.00, 80.00),
		RGB(24.00, 86.00, 53.00),
		RGB(80.00, 58.00, 46.00),
		RGB(40.00, 36.00, 12.00),
		RGB(57.00, 36.00, 51.00),
		RGB(52.00, 11.00, 18.00),
		RGB(98.00, 92.00, 84.00),
		RGB(98.00, 81.00, 69.00),
		RGB(12.00, 75.00, 100.00),
		RGB(50.00, 100.00, 83.00),
		RGB(82.00, 100.00, 8.00),
		RGB(29.00, 44.00, 27.00),
		RGB(91.00, 84.00, 42.00),
		RGB(70.00, 75.00, 71.00),
		RGB(100.00, 60.00, 40.00),
		RGB(99.00, 93.00, 0.00),
		RGB(0.00, 50.00, 100.00),
		RGB(64.00, 100.00, 100.00),
		RGB(54.00, 81.00, 94.00),
		RGB(63.00, 79.00, 95.00),
		RGB(96.00, 76.00, 76.00),
		RGB(100.00, 100.00, 90.00),
		RGB(100.00, 57.00, 69.00),
		RGB(98.00, 91.00, 71.00),
		RGB(49.00, 4.00, 1.00),
		RGB(52.00, 52.00, 51.00),
		RGB(74.00, 83.00, 90.00),
		RGB(62.00, 51.00, 44.00),
		RGB(96.00, 96.00, 86.00),
		RGB(18.00, 35.00, 58.00),
		RGB(61.00, 15.00, 26.00),
		RGB(100.00, 89.00, 77.00),
		RGB(24.00, 17.00, 12.00),
		RGB(59.00, 44.00, 9.00),
		RGB(79.00, 88.00, 5.00),
		RGB(0.00, 0.00, 0.00),
		RGB(24.00, 5.00, 1.00),
		RGB(33.00, 38.00, 44.00),
		RGB(23.00, 24.00, 21.00),
		RGB(75.00, 69.00, 70.00),
		RGB(100.00, 92.00, 80.00),
		RGB(65.00, 44.00, 39.00),
		RGB(19.00, 55.00, 91.00),
		RGB(67.00, 90.00, 93.00),
		RGB(40.00, 0.00, 0.00),
		RGB(0.00, 0.00, 100.00),
		RGB(12.00, 46.00, 100.00),
		RGB(0.00, 58.00, 69.00),
		RGB(0.00, 53.00, 74.00),
		RGB(0.00, 9.00, 66.00),
		RGB(20.00, 20.00, 60.00),
		RGB(64.00, 64.00, 82.00),
		RGB(40.00, 60.00, 80.00),
		RGB(36.00, 68.00, 93.00),
		RGB(7.00, 38.00, 50.00),
		RGB(54.00, 17.00, 89.00),
		RGB(31.00, 45.00, 65.00),
		RGB(24.00, 41.00, 91.00),
		RGB(87.00, 36.00, 51.00),
		RGB(47.00, 27.00, 23.00),
		RGB(89.00, 85.00, 79.00),
		RGB(80.00, 25.00, 33.00),
		RGB(85.00, 57.00, 94.00),
		RGB(100.00, 67.00, 11.00),
		RGB(80.00, 50.00, 20.00),
		RGB(69.00, 43.00, 30.00),
		RGB(48.00, 71.00, 38.00),
		RGB(100.00, 78.00, 50.00),
		RGB(50.00, 0.00, 13.00),
		RGB(87.00, 72.00, 53.00),
		RGB(63.00, 48.00, 45.00),
		RGB(80.00, 33.00, 0.00),
		RGB(91.00, 45.00, 32.00),
		RGB(54.00, 20.00, 14.00),
		RGB(74.00, 20.00, 64.00),
		RGB(44.00, 16.00, 39.00),
		RGB(37.00, 62.00, 63.00),
		RGB(57.00, 64.00, 69.00),
		RGB(0.00, 42.00, 24.00),
		RGB(93.00, 53.00, 18.00),
		RGB(65.00, 48.00, 36.00),
		RGB(29.00, 21.00, 13.00),
		RGB(64.00, 76.00, 68.00),
		RGB(76.00, 60.00, 42.00),
		RGB(94.00, 73.00, 80.00),
		RGB(100.00, 100.00, 60.00),
		RGB(100.00, 94.00, 0.00),
		RGB(89.00, 44.00, 48.00),
		RGB(77.00, 12.00, 23.00),
		RGB(0.00, 80.00, 60.00),
		RGB(59.00, 0.00, 9.00),
		RGB(84.00, 0.00, 25.00),
		RGB(100.00, 65.00, 79.00),
		RGB(70.00, 11.00, 11.00),
		RGB(34.00, 63.00, 83.00),
		RGB(93.00, 57.00, 13.00),
		RGB(44.00, 21.00, 26.00),
		RGB(79.00, 35.00, 29.00),
		RGB(67.00, 88.00, 69.00),
		RGB(70.00, 100.00, 100.00),
		RGB(87.00, 19.00, 39.00),
		RGB(0.00, 48.00, 65.00),
		RGB(16.00, 32.00, 75.00),
		RGB(43.00, 61.00, 76.00),
		RGB(11.00, 67.00, 84.00),
		RGB(97.00, 91.00, 81.00),
		RGB(95.00, 87.00, 81.00),
		RGB(21.00, 27.00, 31.00),
		RGB(90.00, 56.00, 67.00),
		RGB(50.00, 100.00, 0.00),
		RGB(100.00, 72.00, 77.00),
		RGB(58.00, 27.00, 21.00),
		RGB(89.00, 24.00, 16.00),
		RGB(87.00, 44.00, 63.00),
		RGB(67.00, 22.00, 12.00),
		RGB(52.00, 38.00, 53.00),
		RGB(100.00, 70.00, 0.00),
		RGB(48.00, 25.00, 0.00),
		RGB(82.00, 41.00, 12.00),
		RGB(60.00, 51.00, 48.00),
		RGB(89.00, 26.00, 20.00),
		RGB(80.00, 38.00, 49.00),
		RGB(89.00, 82.00, 4.00),
		RGB(62.00, 66.00, 12.00),
		RGB(50.00, 9.00, 20.00),
		RGB(44.00, 31.00, 22.00),
		RGB(73.00, 85.00, 92.00),
		RGB(97.00, 51.00, 47.00),
		RGB(55.00, 57.00, 67.00),
		RGB(72.00, 45.00, 20.00),
		RGB(85.00, 54.00, 40.00),
		RGB(68.00, 44.00, 41.00),
		RGB(80.00, 43.00, 32.00),
		RGB(60.00, 40.00, 40.00),
		RGB(100.00, 22.00, 0.00),
		RGB(100.00, 50.00, 31.00),
		RGB(97.00, 51.00, 47.00),
		RGB(54.00, 25.00, 27.00),
		RGB(98.00, 93.00, 36.00),
		RGB(39.00, 58.00, 93.00),
		RGB(100.00, 97.00, 86.00),
		RGB(18.00, 18.00, 53.00),
		RGB(100.00, 97.00, 91.00),
		RGB(51.00, 38.00, 24.00),
		RGB(100.00, 74.00, 85.00),
		RGB(100.00, 99.00, 82.00),
		RGB(86.00, 8.00, 24.00),
		RGB(62.00, 11.00, 20.00),
		RGB(96.00, 96.00, 96.00),
		RGB(0.00, 100.00, 100.00),
		RGB(0.00, 72.00, 92.00),
		RGB(35.00, 26.00, 49.00),
		RGB(100.00, 83.00, 0.00),
		RGB(96.00, 44.00, 63.00),
		RGB(40.00, 26.00, 13.00),
		RGB(36.00, 22.00, 33.00),
		RGB(0.00, 55.00, 55.00),
		RGB(33.00, 41.00, 47.00),
		RGB(72.00, 53.00, 4.00),
		RGB(0.00, 39.00, 0.00),
		RGB(10.00, 14.00, 13.00),
		RGB(74.00, 72.00, 42.00),
		RGB(28.00, 24.00, 20.00),
		RGB(33.00, 24.00, 22.00),
		RGB(55.00, 0.00, 55.00),
		RGB(33.00, 42.00, 18.00),
		RGB(100.00, 55.00, 0.00),
		RGB(60.00, 20.00, 80.00),
		RGB(19.00, 10.00, 20.00),
		RGB(55.00, 0.00, 0.00),
		RGB(91.00, 59.00, 48.00),
		RGB(56.00, 74.00, 56.00),
		RGB(24.00, 8.00, 8.00),
		RGB(55.00, 75.00, 84.00),
		RGB(28.00, 24.00, 55.00),
		RGB(18.00, 31.00, 31.00),
		RGB(9.00, 45.00, 27.00),
		RGB(0.00, 81.00, 82.00),
		RGB(58.00, 0.00, 83.00),
		RGB(33.00, 33.00, 33.00),
		RGB(85.00, 20.00, 53.00),
		RGB(98.00, 84.00, 65.00),
		RGB(73.00, 31.00, 28.00),
		RGB(0.00, 29.00, 29.00),
		RGB(100.00, 8.00, 58.00),
		RGB(100.00, 60.00, 20.00),
		RGB(0.00, 75.00, 100.00),
		RGB(29.00, 39.00, 42.00),
		RGB(49.00, 37.00, 38.00),
		RGB(8.00, 38.00, 74.00),
		RGB(13.00, 26.00, 71.00),
		RGB(76.00, 60.00, 42.00),
		RGB(93.00, 79.00, 69.00),
		RGB(41.00, 41.00, 41.00),
		RGB(12.00, 56.00, 100.00),
		RGB(29.00, 25.00, 16.00),
		RGB(0.00, 0.00, 61.00),
		RGB(94.00, 87.00, 73.00),
		RGB(33.00, 36.00, 31.00),
		RGB(76.00, 70.00, 50.00),
		RGB(11.00, 11.00, 11.00),
		RGB(38.00, 25.00, 32.00),
		RGB(94.00, 92.00, 84.00),
		RGB(80.00, 100.00, 0.00),
		RGB(75.00, 0.00, 100.00),
		RGB(56.00, 0.00, 100.00),
		RGB(31.00, 78.00, 47.00),
		RGB(42.00, 19.00, 51.00),
		RGB(71.00, 51.00, 58.00),
		RGB(67.00, 29.00, 32.00),
		RGB(80.00, 28.00, 29.00),
		RGB(34.00, 24.00, 36.00),
		RGB(0.00, 100.00, 25.00),
		RGB(59.00, 78.00, 64.00),
		RGB(76.00, 60.00, 42.00),
		RGB(50.00, 9.00, 9.00),
		RGB(71.00, 20.00, 54.00),
		RGB(87.00, 32.00, 52.00),
		RGB(90.00, 67.00, 44.00),
		RGB(31.00, 47.00, 26.00),
		RGB(42.00, 33.00, 12.00),
		RGB(100.00, 33.00, 44.00),
		RGB(41.00, 19.00, 41.00),
		RGB(70.00, 13.00, 13.00),
		RGB(81.00, 13.00, 16.00),
		RGB(89.00, 35.00, 13.00),
		RGB(93.00, 86.00, 51.00),
		RGB(64.00, 0.00, 43.00),
		RGB(100.00, 98.00, 94.00),
		RGB(13.00, 55.00, 13.00),
		RGB(65.00, 48.00, 36.00),
		RGB(52.00, 43.00, 30.00),
		RGB(0.00, 45.00, 73.00),
		RGB(99.00, 25.00, 57.00),
		RGB(53.00, 38.00, 56.00),
		RGB(62.00, 99.00, 22.00),
		RGB(83.00, 45.00, 83.00),
		RGB(99.00, 42.00, 62.00),
		RGB(78.00, 17.00, 28.00),
		RGB(47.00, 71.00, 100.00),
		RGB(53.00, 2.00, 81.00),
		RGB(33.00, 29.00, 8.00)
	};
} // namespace Gui

