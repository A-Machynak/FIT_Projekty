
#include "window_data.h"

namespace Gui
{
	WindowData::WindowData(int width, int height)
		: Width(width), Height(height), Bytes(new GLubyte[width * height * 3])
	{
		std::fill(Bytes, Bytes + (Width*Height*3), 255); // nastavíme na 255 (bílá)
	}

	WindowData::~WindowData()
	{
		delete[] Bytes;
	}

	bool WindowData::SetPixel(int x, int y, RGB color)
	{
		if (x < 0 || x >= Width || y < 0 || y >= Height) {
			return false;
		}

		const auto start = (x + y * Width) * 3;
		Bytes[start] = color.R;
		Bytes[start + 1] = color.G;
		Bytes[start + 2] = color.B;
		return true;
	}

	void WindowData::SetPixelUnsafe(int x, int y, RGB color)
	{
		const auto start = (x + y * Width) * 3;
		Bytes[start] = color.R;
		Bytes[start + 1] = color.G;
		Bytes[start + 2] = color.B;
	}

	RGB WindowData::GetPixel(int x, int y, bool * err) const
	{
		if (x < 0 || x >= Width || y < 0 || y >= Height) {
			if (err != nullptr) {
				*err = true;
			}
			return RgbBlack;
		}

		const auto start = (x + y * Width) * 3;
		return RGB(Bytes[start], Bytes[start + 1], Bytes[start + 2]);
	}

	RGB WindowData::GetPixelUnsafe(int x, int y) const
	{
		const auto start = (x + y * Width) * 3;
		return RGB(Bytes[start], Bytes[start + 1], Bytes[start + 2]);
	}
} // namespace Gui
