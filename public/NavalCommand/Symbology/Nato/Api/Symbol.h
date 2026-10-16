#pragma once

#include <NavalCommand/Symbology/Nato/Api/Amplifiers.h>
#include <NavalCommand/Symbology/Nato/Api/Sidc.h>
#include <optional>

namespace NavalCommand::Symbology::Nato::Api
{
	struct Symbol
	{
		Sidc Identification;
		Amplifiers ExternalAmplifiers;
		// Retains the certainty of source destruction data when a SIDC's Present status cannot express it.
		std::optional<bool> IsDestroyed;
	};
}
