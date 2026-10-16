#include <NavalCommand/Symbology/Nato/Api/Sidc.h>
#include <NavalCommand/Symbology/Nato/Api/SymbolConverter.h>
#include <array>
#include <gtest/gtest.h>

namespace NavalCommand::Symbology::Nato::Api
{
	TEST(SidcTest, ValidatesSubsurfaceLocalModifierAssignmentsAndApplicability)
	{
		SidcFields fields;
		fields.SymbolSetValue = SymbolSet::SeaSubsurface;
		fields.Entity = 11;
		fields.EntityType = 1;
		for (std::uint8_t modifier = 0; modifier < 100; ++modifier)
		{
			fields.Modifier1 = modifier;
			const bool assigned = modifier == 0 || modifier == 2 || (modifier >= 13 && modifier <= 18);
			EXPECT_EQ(Sidc::Create(fields).has_value(), assigned) << static_cast<int>(modifier);
		}
		fields.Modifier1 = 19;
		fields.Entity = 13;
		EXPECT_TRUE(Sidc::Create(fields));
		fields.Modifier1 = 2;
		EXPECT_FALSE(Sidc::Create(fields));
		fields.Entity = 11;
		fields.Modifier1 = 0;
		for (std::uint8_t modifier = 0; modifier < 100; ++modifier)
		{
			fields.Modifier2 = modifier;
			EXPECT_EQ(Sidc::Create(fields).has_value(), modifier <= 13);
		}
		fields.Modifier2 = 0;
		fields.Modifier1 = 51;
		fields.ThirdTen[0] = '1';
		EXPECT_TRUE(Sidc::Create(fields)); // Common ASW 151, not local 51.
		EXPECT_FALSE(Sidc::Parse("140335000011010008000000000000"));
	}

	TEST(SidcTest, PreservesAllThirtyPositions)
	{
		constexpr std::string_view example{"140310021612040100060100000000"};
		auto sidc = Sidc::Parse(example);

		ASSERT_TRUE(sidc);
		EXPECT_EQ(sidc->ToString(), example);
	}

	TEST(SidcTest, RejectsOtherVersionsAndMalformedExtensions)
	{
		auto oldVersion = Sidc::Parse("100301000011010000000000000000");
		auto lowercaseExtension = Sidc::Parse("14030100001101000000000000000a");

		EXPECT_FALSE(oldVersion);
		EXPECT_EQ(oldVersion.error().Code, SidcErrorCode::UnsupportedVersion);
		EXPECT_FALSE(lowercaseExtension);
		EXPECT_EQ(lowercaseExtension.error().Code, SidcErrorCode::InvalidCharacter);
	}

	TEST(SidcTest, RejectsInvalidCoreFieldValues)
	{
		auto invalidContext = Sidc::Parse("149301000011010000000000000000");
		auto invalidIdentity = Sidc::Parse("140901000011010000000000000000");
		auto invalidSymbolSet = Sidc::Parse("140399000011010000000000000000");

		EXPECT_EQ(invalidContext.error().Code, SidcErrorCode::InvalidContext);
		EXPECT_EQ(invalidIdentity.error().Code, SidcErrorCode::InvalidIdentity);
		EXPECT_EQ(invalidSymbolSet.error().Code, SidcErrorCode::UnsupportedSymbolSet);
	}

	TEST(SidcTest, AcceptsCallerResolvedAssumedAndSuspectIdentities)
	{
		SidcFields fields;
		fields.SymbolSetValue = SymbolSet::Air;
		fields.Identity = StandardIdentity::AssumedFriendly;
		const auto assumedFriendly = Sidc::Create(fields);

		fields.Identity = StandardIdentity::Suspect;
		const auto suspect = Sidc::Create(fields);

		ASSERT_TRUE(assumedFriendly);
		ASSERT_TRUE(suspect);
		EXPECT_EQ(assumedFriendly->Fields().Identity, StandardIdentity::AssumedFriendly);
		EXPECT_EQ(suspect->Fields().Identity, StandardIdentity::Suspect);
	}

	TEST(SidcTest, AcceptsEveryDeclaredSymbolSet)
	{
		constexpr std::array symbolSets{SymbolSet::Unknown,
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

		for (const SymbolSet symbolSet : symbolSets)
		{
			SidcFields fields;
			fields.SymbolSetValue = symbolSet;
			const auto sidc = Sidc::Create(fields);
			ASSERT_TRUE(sidc) << static_cast<int>(symbolSet);
			const auto parsed = Sidc::Parse(sidc->ToString());
			ASSERT_TRUE(parsed) << static_cast<int>(symbolSet);
			EXPECT_EQ(parsed->Fields().SymbolSetValue, symbolSet);
		}
	}

	TEST(SidcTest, CreateUsesRealityAndTheStandardDestroyedStatusWhenRequested)
	{
		SidcFields fields;
		fields.Identity = StandardIdentity::Friendly;
		fields.SymbolSetValue = SymbolSet::SeaSubsurface;
		fields.Status = SymbolStatus::PresentDestroyed;
		fields.Entity = 11;
		fields.EntityType = 1;

		auto sidc = Sidc::Create(fields);

		ASSERT_TRUE(sidc);
		EXPECT_EQ(sidc->ToString(), "140335400011010000000000000000");
		EXPECT_EQ(sidc->Fields().ContextValue, Context::Reality);
		EXPECT_EQ(sidc->Fields().Status, SymbolStatus::PresentDestroyed);
	}

	TEST(SidcTest, RejectsReservedThirdTenDigitsAndNonDecimalCoreAssignments)
	{
		auto reservedThirdTen = Sidc::Parse("140301000011010000000012000000");
		auto alphabeticCore = Sidc::Parse("140301000A11010000000000000000");

		ASSERT_FALSE(reservedThirdTen);
		EXPECT_EQ(reservedThirdTen.error().Code, SidcErrorCode::InvalidCombination);
		ASSERT_FALSE(alphabeticCore);
		EXPECT_EQ(alphabeticCore.error().Code, SidcErrorCode::InvalidFieldValue);
	}

	TEST(SymbolConverterTest, CreatesRendererNeutralSymbolsWithoutDomainObjects)
	{
		SidcFields identification;
		identification.SymbolSetValue = SymbolSet::SeaSurface;
		identification.Entity = 12;
		identification.EntityType = 1;
		Amplifiers amplifiers;
		amplifiers.UniqueDesignation = "CVN-72";
		amplifiers.CourseOverGroundRadians = 1.25;
		amplifiers.SpeedMetersPerSecond = 12.0;

		auto symbol = SymbolConverter::Create(identification, amplifiers);

		ASSERT_TRUE(symbol);
		EXPECT_EQ(symbol->Identification.Fields().ContextValue, Context::Reality);
		EXPECT_EQ(symbol->Identification.Fields().SymbolSetValue, SymbolSet::SeaSurface);
		EXPECT_EQ(symbol->ExternalAmplifiers.UniqueDesignation, "CVN-72");
		EXPECT_EQ(symbol->ExternalAmplifiers.CourseOverGroundRadians, 1.25);
		EXPECT_FALSE(symbol->IsDestroyed.has_value());
	}

	TEST(SymbolConverterTest, ParsesCanonicalSidcAndPreservesSuppliedAmplifiers)
	{
		Amplifiers amplifiers;
		amplifiers.HeadingRadians = 0.75;
		const auto symbol = SymbolConverter::Parse("140310021612040100060100000000", amplifiers);

		ASSERT_TRUE(symbol);
		EXPECT_EQ(symbol->Identification.ToString(), "140310021612040100060100000000");
		EXPECT_EQ(symbol->ExternalAmplifiers.HeadingRadians, 0.75);
	}

	TEST(SymbolConverterTest, RejectsDestructionMetadataThatConflictsWithTheSidcStatus)
	{
		SidcFields fields;
		fields.Status = SymbolStatus::PresentDestroyed;

		const auto contradiction = SymbolConverter::Create(fields, {}, false);
		EXPECT_FALSE(contradiction);
		EXPECT_EQ(contradiction.error().Code, SidcErrorCode::InvalidCombination);

		const auto coherent = SymbolConverter::Create(fields, {}, true);
		ASSERT_TRUE(coherent);
		EXPECT_TRUE(coherent->IsDestroyed.value());

		const auto parsedContradiction = SymbolConverter::Parse("140310021612040100060100000000", {}, true);
		EXPECT_FALSE(parsedContradiction);
		EXPECT_EQ(parsedContradiction.error().Code, SidcErrorCode::InvalidCombination);
	}
}
