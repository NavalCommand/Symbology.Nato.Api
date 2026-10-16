#pragma once

#include <NavalCommand/Symbology/Nato/Api/Color.h>
#include <NavalCommand/Symbology/Nato/Api/StandardIdentity.h>

namespace NavalCommand::Symbology::Nato::Api
{
	struct AffiliationPalette
	{
		Color Friendly{128, 224, 255};
		Color Hostile{255, 128, 128};
		Color Neutral{170, 255, 170};
		Color Unknown{255, 255, 128};

		constexpr Color Resolve(StandardIdentity identity) const noexcept
		{
			switch (identity)
			{
			case StandardIdentity::Friendly:
			case StandardIdentity::AssumedFriendly:
				return Friendly;
			case StandardIdentity::Hostile:
			case StandardIdentity::Suspect:
				return Hostile;
			case StandardIdentity::Neutral:
				return Neutral;
			default:
				return Unknown;
			}
		}
	};
}
