#pragma once

#include <NavalCommand/Symbology/Nato/Api/Context.h>
#include <NavalCommand/Symbology/Nato/Api/HeadquartersTaskForceDummy.h>
#include <NavalCommand/Symbology/Nato/Api/SidcError.h>
#include <NavalCommand/Symbology/Nato/Api/StandardIdentity.h>
#include <NavalCommand/Symbology/Nato/Api/SymbolSet.h>
#include <NavalCommand/Symbology/Nato/Api/SymbolStatus.h>
#include <array>
#include <cstdint>
#include <expected>
#include <string>
#include <string_view>

namespace NavalCommand::Symbology::Nato::Api
{
	struct SidcFields
	{
		Context ContextValue{Context::Reality};
		StandardIdentity Identity{StandardIdentity::Unknown};
		SymbolSet SymbolSetValue{SymbolSet::Unknown};
		SymbolStatus Status{SymbolStatus::Present};
		HeadquartersTaskForceDummy Headquarters{HeadquartersTaskForceDummy::None};
		std::uint8_t Amplifier{};
		std::uint8_t Entity{};
		std::uint8_t EntityType{};
		std::uint8_t EntitySubtype{};
		std::uint8_t Modifier1{};
		std::uint8_t Modifier2{};
		std::array<char, 10> ThirdTen{'0', '0', '0', '0', '0', '0', '0', '0', '0', '0'};
	};

	// APP-06(E), Edition E, Version 1 SIDC. The first 20 positions are the
	// typed core; all ten extension positions are retained exactly as hex digits.
	class Sidc final
	{
		SidcFields m_Fields;

		explicit Sidc(SidcFields fields);

	public:
		static std::expected<Sidc, SidcError> Create(SidcFields fields);
		static std::expected<Sidc, SidcError> Parse(std::string_view code);

		std::string ToString() const;
		const SidcFields& Fields() const noexcept;
	};
}
