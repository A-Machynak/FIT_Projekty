
#include "img_export.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

namespace Gui
{
	void ExportPng(const std::string & fileName, int width, int height, const void * data)
	{
		stbi_flip_vertically_on_write(true);
		stbi_write_png((fileName + ".png").c_str(), width, height, 3, data, width * 3);
	}
} // namespace Gui
