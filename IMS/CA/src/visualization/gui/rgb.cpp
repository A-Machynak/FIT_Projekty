
#include "rgb.h"

namespace Gui
{
	bool RGB::operator==(const RGB & other) const
	{
		return R == other.R && G == other.G && B == other.B;
	}

	std::string RGB::ToString() const
	{
		return "{ " + std::to_string(static_cast<int>(R))
			+ " " + std::to_string(static_cast<int>(G))
			+ " " + std::to_string(static_cast<int>(B))
			+ " }";
	}
} // namespace Gui
