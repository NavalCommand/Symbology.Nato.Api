#include <NavalCommand/Symbology/Nato/Api/SymbolConverter.h>
#include <utility>

namespace NavalCommand::Symbology::Nato::Api
{
	std::expected<Symbol, SidcError> SymbolConverter::Create(
		SidcFields identification,
		Amplifiers amplifiers,
		std::optional<bool> isDestroyed)
	{
		auto sidc = Sidc::Create(std::move(identification));
		if (!sidc)
		{
			return std::unexpected(sidc.error());
		}
		if (isDestroyed && *isDestroyed != (sidc->Fields().Status == SymbolStatus::PresentDestroyed))
		{
			return std::unexpected(SidcError{
				SidcErrorCode::InvalidCombination,
				"Explicit destruction metadata must agree with the SIDC operational status."});
		}
		return Symbol{std::move(*sidc), std::move(amplifiers), isDestroyed};
	}

	std::expected<Symbol, SidcError> SymbolConverter::Parse(
		std::string_view identification,
		Amplifiers amplifiers,
		std::optional<bool> isDestroyed)
	{
		auto sidc = Sidc::Parse(identification);
		if (!sidc)
		{
			return std::unexpected(sidc.error());
		}
		if (isDestroyed && *isDestroyed != (sidc->Fields().Status == SymbolStatus::PresentDestroyed))
		{
			return std::unexpected(SidcError{
				SidcErrorCode::InvalidCombination,
				"Explicit destruction metadata must agree with the SIDC operational status."});
		}
		return Symbol{std::move(*sidc), std::move(amplifiers), isDestroyed};
	}
}
