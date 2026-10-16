#pragma once

#include <cstdint>

namespace NavalCommand::Symbology::Nato::Api
{
	struct Color
	{
		std::uint8_t Red{};
		std::uint8_t Green{};
		std::uint8_t Blue{};
		std::uint8_t Alpha{255};
		bool operator==(const Color&) const = default;
	};
}
