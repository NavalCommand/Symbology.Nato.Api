#pragma once

#include <cstdint>

namespace NavalCommand::Symbology::Nato::Api
{
	enum class StandardIdentity : std::uint8_t
	{
		Pending = 0,
		Unknown = 1,
		AssumedFriendly = 2,
		Friendly = 3,
		Neutral = 4,
		Suspect = 5,
		Hostile = 6
	};
}
