#pragma once

#include <cstdint>

namespace NavalCommand::Symbology::Nato::Api
{
	enum class SymbolStatus : std::uint8_t
	{
		Present = 0,
		PlannedAnticipatedSuspect = 1,
		PresentFullyCapable = 2,
		PresentDamaged = 3,
		PresentDestroyed = 4,
		PresentFullToCapacity = 5
	};
}
