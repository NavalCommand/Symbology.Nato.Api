#pragma once

#include <cstdint>

namespace NavalCommand::Symbology::Nato::Api
{
	enum class Context : std::uint8_t
	{
		Reality = 0,
		Exercise = 1,
		Simulation = 2
	};
}
