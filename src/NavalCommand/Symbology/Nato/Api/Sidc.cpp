#include <NavalCommand/Symbology/Nato/Api/Sidc.h>
#include <algorithm>
#include <array>
#include <string_view>
#include <utility>

namespace NavalCommand::Symbology::Nato::Api
{
	namespace
	{
		constexpr std::array SupportedSymbolSets{SymbolSet::Unknown,
												 SymbolSet::Air,
												 SymbolSet::AirMissile,
												 SymbolSet::Space,
												 SymbolSet::SpaceMissile,
												 SymbolSet::LandUnit,
												 SymbolSet::LandCivilianUnit,
												 SymbolSet::LandEquipment,
												 SymbolSet::LandInstallation,
												 SymbolSet::ControlMeasure,
												 SymbolSet::DismountedIndividual,
												 SymbolSet::SeaSurface,
												 SymbolSet::SeaSubsurface,
												 SymbolSet::MineWarfare,
												 SymbolSet::Activities,
												 SymbolSet::Atmospheric,
												 SymbolSet::Oceanographic,
												 SymbolSet::MeteorologicalSpace,
												 SymbolSet::SignalsIntelligenceSpace,
												 SymbolSet::SignalsIntelligenceAir,
												 SymbolSet::SignalsIntelligenceLandEquipment,
												 SymbolSet::SignalsIntelligenceSeaSurface,
												 SymbolSet::SignalsIntelligenceSeaSubsurface,
												 SymbolSet::Cyberspace,
												 SymbolSet::CyberspaceSpace,
												 SymbolSet::CyberspaceAir,
												 SymbolSet::CyberspaceLandUnit,
												 SymbolSet::CyberspaceLandEquipment,
												 SymbolSet::CyberspaceLandInstallation,
												 SymbolSet::CyberspaceDismountedIndividual,
												 SymbolSet::CyberspaceSeaSurface,
												 SymbolSet::CyberspaceSeaSubsurface};
		constexpr std::string_view SIDC_VERSION{"14"};

		SidcError Error(SidcErrorCode code, std::string message)
		{
			return {code, std::move(message)};
		}

		bool IsHexDigit(char character)
		{
			return (character >= '0' && character <= '9') || (character >= 'A' && character <= 'F');
		}

		void AppendTwoDigits(std::string& result, std::uint8_t value)
		{
			result.push_back(static_cast<char>('0' + value / 10));
			result.push_back(static_cast<char>('0' + value % 10));
		}
	}

	Sidc::Sidc(SidcFields fields) : m_Fields{std::move(fields)}
	{
	}

	std::expected<Sidc, SidcError> Sidc::Create(SidcFields fields)
	{
		if (fields.ContextValue > Context::Simulation)
		{
			return std::unexpected(Error(SidcErrorCode::InvalidContext, "Context must be Reality, Exercise, or Simulation."));
		}
		if (fields.Identity > StandardIdentity::Hostile)
		{
			return std::unexpected(Error(SidcErrorCode::InvalidIdentity, "Standard Identity must use an APP-06(E)(1) identity value from 0 through 6."));
		}
		if (std::find(SupportedSymbolSets.begin(), SupportedSymbolSets.end(), fields.SymbolSetValue) == SupportedSymbolSets.end())
		{
			return std::unexpected(Error(SidcErrorCode::UnsupportedSymbolSet, "Symbol Set is not assigned in APP-06(E), Edition E, Version 1."));
		}
		if (fields.Status > SymbolStatus::PresentFullToCapacity)
		{
			return std::unexpected(Error(SidcErrorCode::InvalidStatus, "Status must use an APP-06(E)(1) value from 0 through 5."));
		}
		if (fields.Headquarters > HeadquartersTaskForceDummy::FeintOrDummyTaskForceHeadquarters)
		{
			return std::unexpected(Error(SidcErrorCode::InvalidHeadquartersTaskForceDummy, "Headquarters/Task Force/Dummy must use a value from 0 through 7."));
		}
		if (fields.Amplifier > 99 || fields.Entity > 99 || fields.EntityType > 99 || fields.EntitySubtype > 99 || fields.Modifier1 > 99 || fields.Modifier2 > 99)
		{
			return std::unexpected(Error(SidcErrorCode::InvalidFieldValue, "Two-position numeric fields must be from 00 through 99."));
		}
		if (!std::all_of(fields.ThirdTen.begin(), fields.ThirdTen.end(), IsHexDigit))
		{
			return std::unexpected(Error(SidcErrorCode::InvalidCharacter, "The third ten positions must contain uppercase hexadecimal characters."));
		}
		if (!std::all_of(fields.ThirdTen.begin() + 2,
						 fields.ThirdTen.begin() + 7,
						 [](char character)
						 {
							 return character == '0';
						 }))
		{
			return std::unexpected(Error(SidcErrorCode::InvalidCombination, "APP-06(E)(1) reserves third-ten positions 23 through 27; they must be zero."));
		}
		if ((fields.Entity == 0 && (fields.EntityType != 0 || fields.EntitySubtype != 0)) || (fields.EntityType == 0 && fields.EntitySubtype != 0))
		{
			return std::unexpected(Error(SidcErrorCode::InvalidCombination, "Entity, entity type, and entity subtype must follow the APP-06 hierarchy."));
		}
		if (fields.SymbolSetValue == SymbolSet::SeaSubsurface)
		{
			// APP-06(E)(1), A-42/A-43. Common modifiers use third-ten extension digits.
			const bool submarine = fields.Entity == 11 && fields.EntityType == 1;
			if (fields.ThirdTen[0] == '0' && fields.Modifier1 != 0)
			{
				const bool submarineModifier = fields.Modifier1 == 2 || (fields.Modifier1 >= 13 && fields.Modifier1 <= 18);
				const bool antiTorpedo = fields.Modifier1 == 19 && fields.Entity == 13 && fields.EntityType == 1;
				if (!(submarineModifier && submarine) && !antiTorpedo)
				{
					return std::unexpected(Error(SidcErrorCode::InvalidCombination, "Unassigned or inapplicable Sea Subsurface sector-1 modifier (Table A-42)."));
				}
			}
			if (fields.ThirdTen[1] == '0' && fields.Modifier2 != 0 && (!submarine || fields.Modifier2 > 13))
			{
				return std::unexpected(Error(SidcErrorCode::InvalidCombination, "Unassigned or inapplicable Sea Subsurface propulsion modifier (Table A-43)."));
			}
		}
		return Sidc{std::move(fields)};
	}

	std::expected<Sidc, SidcError> Sidc::Parse(std::string_view code)
	{
		if (code.size() != 30)
		{
			return std::unexpected(Error(SidcErrorCode::InvalidLength, "APP-06(E)(1) SIDC must contain exactly 30 positions."));
		}
		if (!std::all_of(code.begin(), code.end(), IsHexDigit))
		{
			return std::unexpected(Error(SidcErrorCode::InvalidCharacter, "A SIDC position must be an uppercase hexadecimal character from 0 through F."));
		}
		if (code.substr(0, SIDC_VERSION.size()) != SIDC_VERSION)
		{
			return std::unexpected(Error(SidcErrorCode::UnsupportedVersion, "Only APP-06(E), Edition E, Version 1 (version code 14) is accepted."));
		}
		if (!std::all_of(code.begin() + 2,
						 code.begin() + 20,
						 [](char character)
						 {
							 return character >= '0' && character <= '9';
						 }))
		{
			return std::unexpected(Error(SidcErrorCode::InvalidFieldValue, "The supported APP-06(E)(1) core field values are decimal assignments."));
		}

		SidcFields fields;
		fields.ContextValue = static_cast<Context>(code[2] - '0');
		fields.Identity = static_cast<StandardIdentity>(code[3] - '0');
		fields.SymbolSetValue = static_cast<SymbolSet>((code[4] - '0') * 10 + code[5] - '0');
		fields.Status = static_cast<SymbolStatus>(code[6] - '0');
		fields.Headquarters = static_cast<HeadquartersTaskForceDummy>(code[7] - '0');
		fields.Amplifier = static_cast<std::uint8_t>((code[8] - '0') * 10 + code[9] - '0');
		fields.Entity = static_cast<std::uint8_t>((code[10] - '0') * 10 + code[11] - '0');
		fields.EntityType = static_cast<std::uint8_t>((code[12] - '0') * 10 + code[13] - '0');
		fields.EntitySubtype = static_cast<std::uint8_t>((code[14] - '0') * 10 + code[15] - '0');
		fields.Modifier1 = static_cast<std::uint8_t>((code[16] - '0') * 10 + code[17] - '0');
		fields.Modifier2 = static_cast<std::uint8_t>((code[18] - '0') * 10 + code[19] - '0');
		std::copy(code.begin() + 20, code.end(), fields.ThirdTen.begin());
		return Create(std::move(fields));
	}

	std::string Sidc::ToString() const
	{
		std::string code;
		code.reserve(30);
		code += SIDC_VERSION;
		code.push_back(static_cast<char>('0' + static_cast<std::uint8_t>(m_Fields.ContextValue)));
		code.push_back(static_cast<char>('0' + static_cast<std::uint8_t>(m_Fields.Identity)));
		AppendTwoDigits(code, static_cast<std::uint8_t>(m_Fields.SymbolSetValue));
		code.push_back(static_cast<char>('0' + static_cast<std::uint8_t>(m_Fields.Status)));
		code.push_back(static_cast<char>('0' + static_cast<std::uint8_t>(m_Fields.Headquarters)));
		AppendTwoDigits(code, m_Fields.Amplifier);
		AppendTwoDigits(code, m_Fields.Entity);
		AppendTwoDigits(code, m_Fields.EntityType);
		AppendTwoDigits(code, m_Fields.EntitySubtype);
		AppendTwoDigits(code, m_Fields.Modifier1);
		AppendTwoDigits(code, m_Fields.Modifier2);
		code.append(m_Fields.ThirdTen.begin(), m_Fields.ThirdTen.end());
		return code;
	}

	const SidcFields& Sidc::Fields() const noexcept
	{
		return m_Fields;
	}
}
