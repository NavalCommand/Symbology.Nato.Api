#pragma once

#include <string>

namespace NavalCommand::Symbology::Nato::Api
{
	enum class SidcErrorCode
	{
		InvalidLength,
		InvalidCharacter,
		UnsupportedVersion,
		InvalidContext,
		InvalidIdentity,
		UnsupportedSymbolSet,
		InvalidStatus,
		InvalidHeadquartersTaskForceDummy,
		InvalidFieldValue,
		InvalidCombination
	};

	struct SidcError
	{
		SidcErrorCode Code;
		std::string Message;
	};
}
