#pragma once

#include <NavalCommand/Symbology/Nato/Api/Amplifiers.h>
#include <NavalCommand/Symbology/Nato/Api/Sidc.h>
#include <NavalCommand/Symbology/Nato/Api/Symbol.h>
#include <expected>
#include <optional>
#include <string_view>

namespace NavalCommand::Symbology::Nato::Api
{
	// Converts standard fields or a canonical SIDC into the renderer-neutral value.
	// This core entry point does not depend on simulation units or observations.
	class SymbolConverter final
	{
	public:
		static std::expected<Symbol, SidcError> Create(
			SidcFields identification,
			Amplifiers amplifiers = {},
			std::optional<bool> isDestroyed = std::nullopt);

		static std::expected<Symbol, SidcError> Parse(
			std::string_view identification,
			Amplifiers amplifiers = {},
			std::optional<bool> isDestroyed = std::nullopt);
	};
}
