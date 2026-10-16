#include <NavalCommand/Symbology/Nato/Api/DomainConverter.h>
#include <array>
#include <gtest/gtest.h>

namespace NavalCommand::Symbology::Nato::Api
{
	TEST(DomainConverterTest, SubmarinePhysicalTypesNeverInferMissionOrPropulsion)
	{
		using Classification::Api::Role;
		using Classification::Api::SubCategory;
		for (auto subcategory : {SubCategory::Submarine, SubCategory::AttackSubmarine, SubCategory::BallisticMissileSubmarine, SubCategory::CruiseMissileSubmarine})
		{
			Units::Api::Instance unit{};
			unit.Type.Value = "submarine";
			Units::Api::Type type{};
			type.Id = unit.Type;
			type.UnitSubCategory = subcategory;
			type.PlatformRoles = {Role::Strike, Role::SurfaceWarfare, Role::AntiSubmarineWarfare};
			const auto symbol = DomainConverter::FromUnit(unit, type, StandardIdentity::Friendly);
			ASSERT_TRUE(symbol);
			EXPECT_EQ(symbol->Identification.ToString(), "140335000011010000000000000000");
			Observations::Api::Instance observation{};
			observation.Type = decltype(observation.Type)::Known<Units::Api::TypeId>(unit.Type);
			ASSERT_TRUE(DomainConverter::FromObservation(observation, type, StandardIdentity::Friendly));
			EXPECT_EQ(DomainConverter::FromObservation(observation, type, StandardIdentity::Friendly)->Identification.ToString(), symbol->Identification.ToString());
			observation.Type = decltype(observation.Type)::Known<SubCategory>(subcategory);
			EXPECT_EQ(DomainConverter::FromObservation(observation, type, StandardIdentity::Friendly)->Identification.ToString(), symbol->Identification.ToString());
			for (const auto& [role, modifier] : std::array{std::pair{Role::AntiSubmarineWarfare, 51}, std::pair{Role::SurfaceWarfare, 55}, std::pair{Role::Strike, 6}})
			{
				type.PrimaryPlatformRole = role;
				const auto specialized = DomainConverter::FromUnit(unit, type, StandardIdentity::Friendly);
				ASSERT_TRUE(specialized);
				const int expectedModifier = subcategory == SubCategory::BallisticMissileSubmarine && role == Role::Strike ? 8 : modifier;
				EXPECT_EQ(specialized->Identification.Fields().Modifier1, expectedModifier);
				EXPECT_EQ(specialized->Identification.Fields().ThirdTen[0], '1');
				EXPECT_EQ(specialized->Identification.Fields().Modifier2, 0);
				// A catalog record cannot add a mission to a partially classified track.
				EXPECT_EQ(DomainConverter::FromObservation(observation, type, StandardIdentity::Friendly)->Identification.Fields().Modifier1, 0);
			}
		}
	}

	TEST(DomainConverterTest, ApplicablePrimaryRolesCoverAllPhysicalSubcategories)
	{
		using Classification::Api::Role;
		using Classification::Api::SubCategory;
		const std::array cases{std::pair{SubCategory::FixedWingAircraft, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::RotaryWingAircraft, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::TiltRotorAircraft, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::CruiseMissile, Role::SurfaceWarfare},
							   std::pair{SubCategory::BallisticMissile, Role::LandAttack},
							   std::pair{SubCategory::AircraftCarrier, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::Cruiser, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::Destroyer, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::Frigate, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::Corvette, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::PatrolVessel, Role::SurfaceWarfare},
							   std::pair{SubCategory::AmphibiousVessel, Role::Transport},
							   std::pair{SubCategory::AuxiliaryVessel, Role::Reconnaissance},
							   std::pair{SubCategory::MerchantVessel, Role::Transport},
							   std::pair{SubCategory::AttackSubmarine, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::BallisticMissileSubmarine, Role::Strike},
							   std::pair{SubCategory::CruiseMissileSubmarine, Role::SurfaceWarfare},
							   std::pair{SubCategory::HeavyweightTorpedo, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::LightweightTorpedo, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::NavalMine, Role::MineWarfare},
							   std::pair{SubCategory::Noisemaker, Role::Decoy},
							   std::pair{SubCategory::SelfPropelledAcousticDecoy, Role::Decoy},
							   std::pair{SubCategory::Sonobuoy, Role::Surveillance},
							   std::pair{SubCategory::GroundVehicle, Role::Transport},
							   std::pair{SubCategory::SurfaceToAirMissileLauncher, Role::AirDefence},
							   std::pair{SubCategory::AntiAircraftGun, Role::AirDefence},
							   std::pair{SubCategory::Radar, Role::Surveillance},
							   std::pair{SubCategory::Airfield, Role::Transport},
							   std::pair{SubCategory::MilitaryInstallation, Role::Surveillance},
							   std::pair{SubCategory::CivilianInstallation, Role::Transport},
							   std::pair{SubCategory::Missile, Role::AirDefence},
							   std::pair{SubCategory::Torpedo, Role::AntiSubmarineWarfare},
							   std::pair{SubCategory::SurfaceVessel, Role::SurfaceWarfare},
							   std::pair{SubCategory::Submarine, Role::AntiSubmarineWarfare}};
		Units::Api::Instance unit{};
		unit.Type.Value = "test";
		for (const auto& [subcategory, role] : cases)
		{
			Units::Api::Type type{};
			type.Id = unit.Type;
			type.UnitSubCategory = subcategory;
			type.PlatformRoles = {role};
			type.PrimaryPlatformRole = role;
			const auto symbol = DomainConverter::FromUnit(unit, type, StandardIdentity::Friendly);
			ASSERT_TRUE(symbol) << static_cast<int>(subcategory);
			const auto& fields = symbol->Identification.Fields();
			const bool mainIconRole = role == Role::Decoy || (role == Role::AirDefence && fields.SymbolSetValue == SymbolSet::LandEquipment);
			EXPECT_TRUE(fields.Modifier1 != 0 || fields.Modifier2 != 0 || mainIconRole) << static_cast<int>(subcategory);
			if (role == Role::AntiSubmarineWarfare && fields.SymbolSetValue != SymbolSet::AirMissile)
			{
				EXPECT_EQ(fields.ThirdTen[0], '1');
				EXPECT_EQ(fields.Modifier1, 51);
			}
			Observations::Api::Instance observation{};
			observation.Type = decltype(observation.Type)::Known<Units::Api::TypeId>(type.Id);
			const auto observed = DomainConverter::FromObservation(observation, type, StandardIdentity::Hostile);
			ASSERT_TRUE(observed);
			EXPECT_EQ(observed->Identification.Fields().Modifier1, fields.Modifier1);
			EXPECT_EQ(observed->Identification.Fields().Modifier2, fields.Modifier2);
		}
		// Animals have no applicable military mission; Other remains an explicit error.
	}

	TEST(DomainConverterTest, RejectsInapplicablePrimaryRatherThanDroppingIt)
	{
		Units::Api::Instance unit{};
		Units::Api::Type type{};
		type.Id = unit.Type;
		type.UnitSubCategory = Classification::Api::SubCategory::Destroyer;
		type.PrimaryPlatformRole = Classification::Api::Role::AerialRefuelling;
		type.PlatformRoles = {*type.PrimaryPlatformRole};
		const auto symbol = DomainConverter::FromUnit(unit, type, StandardIdentity::Friendly);
		ASSERT_FALSE(symbol);
		EXPECT_EQ(symbol.error().Code, ConversionErrorCode::UnsupportedClassification);
	}

	TEST(DomainConverterTest, GuidedMissileCruiserUsesCommonRoleModifier)
	{
		Units::Api::Instance unit{};
		unit.Type.Value = "cruiser";
		Units::Api::Type type{};
		type.Id = unit.Type;
		type.UnitSubCategory = Classification::Api::SubCategory::Cruiser;
		type.PlatformRoles = {Classification::Api::Role::GuidedMissile};
		const auto ordinary = DomainConverter::FromUnit(unit, type, StandardIdentity::Friendly);
		ASSERT_TRUE(ordinary);
		EXPECT_EQ(ordinary->Identification.Fields().Modifier1, 0);
		type.PrimaryPlatformRole = Classification::Api::Role::GuidedMissile;
		const auto guided = DomainConverter::FromUnit(unit, type, StandardIdentity::Friendly);
		ASSERT_TRUE(guided);
		EXPECT_EQ(guided->Identification.Fields().EntitySubtype, 2);
		EXPECT_EQ(guided->Identification.Fields().Modifier1, 33);
		EXPECT_EQ(guided->Identification.Fields().ThirdTen[0], '1');
	}

	TEST(DomainConverterTest, PrimaryRoleAloneSelectsAircraftMissionModifier)
	{
		Units::Api::Instance unit{};
		unit.Type.Value = "aircraft";
		Units::Api::Type type{};
		type.Id = unit.Type;
		type.UnitSubCategory = Classification::Api::SubCategory::FixedWingAircraft;
		type.PlatformRoles = {Classification::Api::Role::AirSuperiority, Classification::Api::Role::ElectronicWarfare, Classification::Api::Role::AirborneEarlyWarning};
		EXPECT_EQ(DomainConverter::FromUnit(unit, type, StandardIdentity::Friendly)->Identification.Fields().Modifier1, 0);
		for (const auto& [role, modifier] : std::array{std::pair{Classification::Api::Role::AirSuperiority, 4},
													   std::pair{Classification::Api::Role::AirborneEarlyWarning, 12},
													   std::pair{Classification::Api::Role::ElectronicWarfare, 16}})
		{
			type.PrimaryPlatformRole = role;
			EXPECT_EQ(DomainConverter::FromUnit(unit, type, StandardIdentity::Friendly)->Identification.Fields().Modifier1, modifier);
			Observations::Api::Instance observation{};
			observation.Type = decltype(observation.Type)::Known<Units::Api::TypeId>(type.Id);
			EXPECT_EQ(DomainConverter::FromObservation(observation, type, StandardIdentity::Hostile)->Identification.Fields().Modifier1, modifier);
		}
	}

	TEST(DomainConverterTest, MapsUnitSubcategoryAndPreservesCourseSeparatelyFromHeading)
	{
		Units::Api::Instance unit{};
		unit.Identity.Value = "unit-1";
		unit.Name = "Contact Alpha";
		unit.Type.Value = "type-1";
		unit.CurrentLoadout.Value = "empty";
		unit.IsDestroyed = false;
		unit.Kinematics.Heading = Physics::Api::Radians{0.25};
		unit.Kinematics.CourseOverGround = Physics::Api::Radians{1.5};
		unit.Kinematics.Speed = Physics::Api::MetersPerSecond{12.0};

		Units::Api::Type type{};
		type.Id = {"type-1"};
		type.UnitSubCategory = Classification::Api::SubCategory::FixedWingAircraft;
		type.PlatformRoles = {Classification::Api::Role::AirborneEarlyWarning};
		Equipment::Api::Loadout loadout{};
		loadout.Id.Value = "empty";
		loadout.Roles = {Classification::Api::Role::ElectronicWarfare};
		type.Loadouts.push_back(loadout);

		auto symbol = DomainConverter::FromUnit(unit, type, StandardIdentity::Friendly);

		ASSERT_TRUE(symbol);
		EXPECT_EQ(symbol->Identification.Fields().SymbolSetValue, SymbolSet::Air);
		EXPECT_EQ(symbol->Identification.Fields().Entity, 11);
		EXPECT_EQ(symbol->Identification.Fields().EntityType, 1);
		EXPECT_EQ(symbol->Identification.Fields().Entity, 11);
		EXPECT_EQ(symbol->Identification.Fields().Status, SymbolStatus::Present);
		EXPECT_EQ(symbol->Identification.Fields().ContextValue, Context::Reality);
		EXPECT_EQ(symbol->ExternalAmplifiers.HeadingRadians, 0.25);
		EXPECT_EQ(symbol->ExternalAmplifiers.CourseOverGroundRadians, 1.5);
		EXPECT_EQ(symbol->ExternalAmplifiers.SpeedMetersPerSecond, 12.0);
		EXPECT_EQ(symbol->Identification.Fields().Modifier1, 0);
		ASSERT_TRUE(symbol->IsDestroyed.has_value());
		EXPECT_FALSE(*symbol->IsDestroyed);
	}

	TEST(DomainConverterTest, LeavesUnknownObservationClassificationGenericAndDestructionUnknown)
	{
		Observations::Api::Instance observation{};
		observation.Identity.Value = "contact-1";
		observation.ObservationName = "Unidentified contact";
		observation.ObserverFaction.Value = "blue";
		observation.Type = decltype(observation.Type)::Unknown();
		observation.IsDestroyed = decltype(observation.IsDestroyed)::Unknown();

		auto symbol = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);

		ASSERT_TRUE(symbol);
		EXPECT_EQ(symbol->Identification.Fields().SymbolSetValue, SymbolSet::Unknown);
		EXPECT_EQ(symbol->Identification.Fields().Entity, 0);
		EXPECT_EQ(symbol->Identification.Fields().Status, SymbolStatus::Present);
		EXPECT_FALSE(symbol->IsDestroyed.has_value());
	}

	TEST(DomainConverterTest, MapsKnownObservationDestructionWithoutGuessingUnknownState)
	{
		Observations::Api::Instance observation{};
		observation.Type = decltype(observation.Type)::Known<Classification::Api::Category>(Classification::Api::Category::Submarine);
		observation.IsDestroyed = decltype(observation.IsDestroyed)::Known(true);
		const auto destroyed = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);

		ASSERT_TRUE(destroyed);
		EXPECT_EQ(destroyed->Identification.Fields().Status, SymbolStatus::PresentDestroyed);
		ASSERT_TRUE(destroyed->IsDestroyed.has_value());
		EXPECT_TRUE(*destroyed->IsDestroyed);

		observation.IsDestroyed = decltype(observation.IsDestroyed)::Known(false);
		const auto notDestroyed = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);

		ASSERT_TRUE(notDestroyed);
		EXPECT_EQ(notDestroyed->Identification.Fields().Status, SymbolStatus::Present);
		ASSERT_TRUE(notDestroyed->IsDestroyed.has_value());
		EXPECT_FALSE(*notDestroyed->IsDestroyed);
	}

	TEST(DomainConverterTest, RejectsATypeDefinitionThatDoesNotMatchTheAuthoritativeUnit)
	{
		Units::Api::Instance unit{};
		unit.Type.Value = "actual-type";
		Units::Api::Type suppliedType{};
		suppliedType.Id.Value = "other-type";

		const auto symbol = DomainConverter::FromUnit(unit, suppliedType, StandardIdentity::Friendly);

		ASSERT_FALSE(symbol);
		EXPECT_EQ(symbol.error().Code, ConversionErrorCode::MismatchedTypeDefinition);
	}

	TEST(DomainConverterTest, PreservesKnownUnitTypeIdWhenItsClassificationRecordIsUnavailable)
	{
		Units::Api::Instance unit{};
		unit.Identity.Value = "unit-perry";
		unit.Name = "USS Perry";
		unit.Type.Value = "vessels/usn_ffg_oliver_hazard_perry";

		const auto symbol = DomainConverter::FromUnit(unit, std::nullopt, StandardIdentity::Friendly);

		ASSERT_TRUE(symbol);
		EXPECT_EQ(symbol->Identification.Fields().SymbolSetValue, SymbolSet::Unknown);
		EXPECT_EQ(symbol->ExternalAmplifiers.NameOrType, "vessels/usn_ffg_oliver_hazard_perry");
	}

	TEST(DomainConverterTest, DoesNotPromoteGenericSubmarineObservationToSpecializedType)
	{
		Observations::Api::Instance observation{};
		observation.Identity.Value = "contact-sub";
		observation.ObservationName = "Submarine track";
		observation.Type = decltype(observation.Type)::Known<Classification::Api::SubCategory>(Classification::Api::SubCategory::Submarine);
		observation.IsDestroyed = decltype(observation.IsDestroyed)::Known(false);

		auto symbol = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);

		ASSERT_TRUE(symbol);
		EXPECT_EQ(symbol->Identification.Fields().SymbolSetValue, SymbolSet::SeaSubsurface);
		EXPECT_EQ(symbol->Identification.Fields().Entity, 11);
		EXPECT_EQ(symbol->Identification.Fields().EntityType, 1);
		EXPECT_EQ(symbol->Identification.Fields().EntitySubtype, 0);
		EXPECT_EQ(symbol->Identification.Fields().Modifier1, 0);
	}

	TEST(DomainConverterTest, MissingExactTypeDefinitionDoesNotUseTheTypeIdentifierAsClassification)
	{
		Observations::Api::Instance observation{};
		observation.Identity.Value = "contact-exact";
		observation.ObservationName = "Exact ID only";
		observation.Type = decltype(observation.Type)::Known<Units::Api::TypeId>(Units::Api::TypeId{"private-id"});
		observation.IsDestroyed = decltype(observation.IsDestroyed)::Known(true);

		auto symbol = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Hostile, Context::Exercise);

		ASSERT_TRUE(symbol);
		EXPECT_EQ(symbol->Identification.Fields().SymbolSetValue, SymbolSet::Unknown);
		EXPECT_EQ(symbol->Identification.Fields().ContextValue, Context::Exercise);
		EXPECT_EQ(symbol->Identification.Fields().Status, SymbolStatus::PresentDestroyed);
		EXPECT_EQ(symbol->ExternalAmplifiers.NameOrType, "private-id");
		ASSERT_TRUE(symbol->IsDestroyed.has_value());
		EXPECT_TRUE(*symbol->IsDestroyed);
	}

	TEST(DomainConverterTest, KnownOtherClassificationIsNotSilentlyChangedToUnknown)
	{
		Observations::Api::Instance observation{};
		observation.Identity.Value = "contact-other";
		observation.ObservationName = "Other";
		observation.Type = decltype(observation.Type)::Known<Classification::Api::SubCategory>(Classification::Api::SubCategory::Other);

		auto symbol = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);

		EXPECT_FALSE(symbol);
		EXPECT_EQ(symbol.error().Code, ConversionErrorCode::UnsupportedClassification);
	}

	TEST(DomainConverterTest, MapsAlignmentOnlyWhenTheCallerSuppliesIt)
	{
		EXPECT_EQ(DomainConverter::FromAlignment(Classification::Api::Alignment::Friendly), StandardIdentity::Friendly);
		EXPECT_EQ(DomainConverter::FromAlignment(Classification::Api::Alignment::Neutral), StandardIdentity::Neutral);
		EXPECT_EQ(DomainConverter::FromAlignment(Classification::Api::Alignment::Hostile), StandardIdentity::Hostile);
		EXPECT_EQ(DomainConverter::FromAlignment(std::nullopt), StandardIdentity::Unknown);
	}

	TEST(DomainConverterTest, CoversEveryDeclaredSubcategoryWithItsDimensionAppropriateSetOrExplicitError)
	{
		using SubCategory = Classification::Api::SubCategory;
		const std::array cases{std::pair{SubCategory::FixedWingAircraft, SymbolSet::Air},
							   std::pair{SubCategory::RotaryWingAircraft, SymbolSet::Air},
							   std::pair{SubCategory::TiltRotorAircraft, SymbolSet::Air},
							   std::pair{SubCategory::CruiseMissile, SymbolSet::AirMissile},
							   std::pair{SubCategory::BallisticMissile, SymbolSet::AirMissile},
							   std::pair{SubCategory::AircraftCarrier, SymbolSet::SeaSurface},
							   std::pair{SubCategory::Cruiser, SymbolSet::SeaSurface},
							   std::pair{SubCategory::Destroyer, SymbolSet::SeaSurface},
							   std::pair{SubCategory::Frigate, SymbolSet::SeaSurface},
							   std::pair{SubCategory::Corvette, SymbolSet::SeaSurface},
							   std::pair{SubCategory::PatrolVessel, SymbolSet::SeaSurface},
							   std::pair{SubCategory::AmphibiousVessel, SymbolSet::SeaSurface},
							   std::pair{SubCategory::AuxiliaryVessel, SymbolSet::SeaSurface},
							   std::pair{SubCategory::MerchantVessel, SymbolSet::SeaSurface},
							   std::pair{SubCategory::AttackSubmarine, SymbolSet::SeaSubsurface},
							   std::pair{SubCategory::BallisticMissileSubmarine, SymbolSet::SeaSubsurface},
							   std::pair{SubCategory::CruiseMissileSubmarine, SymbolSet::SeaSubsurface},
							   std::pair{SubCategory::HeavyweightTorpedo, SymbolSet::SeaSubsurface},
							   std::pair{SubCategory::LightweightTorpedo, SymbolSet::SeaSubsurface},
							   std::pair{SubCategory::NavalMine, SymbolSet::MineWarfare},
							   std::pair{SubCategory::Noisemaker, SymbolSet::SeaSubsurface},
							   std::pair{SubCategory::SelfPropelledAcousticDecoy, SymbolSet::SeaSubsurface},
							   std::pair{SubCategory::Sonobuoy, SymbolSet::SeaSurface},
							   std::pair{SubCategory::GroundVehicle, SymbolSet::LandEquipment},
							   std::pair{SubCategory::SurfaceToAirMissileLauncher, SymbolSet::LandEquipment},
							   std::pair{SubCategory::AntiAircraftGun, SymbolSet::LandEquipment},
							   std::pair{SubCategory::Radar, SymbolSet::LandEquipment},
							   std::pair{SubCategory::Airfield, SymbolSet::LandInstallation},
							   std::pair{SubCategory::MilitaryInstallation, SymbolSet::LandInstallation},
							   std::pair{SubCategory::CivilianInstallation, SymbolSet::LandInstallation},
							   std::pair{SubCategory::Missile, SymbolSet::AirMissile},
							   std::pair{SubCategory::Torpedo, SymbolSet::SeaSubsurface},
							   std::pair{SubCategory::SurfaceVessel, SymbolSet::SeaSurface},
							   std::pair{SubCategory::Submarine, SymbolSet::SeaSubsurface},
							   std::pair{SubCategory::Whale, SymbolSet::SeaSubsurface}};
		Observations::Api::Instance observation{};
		for (const auto& [subcategory, expectedSet] : cases)
		{
			observation.Type = decltype(observation.Type)::Known<SubCategory>(subcategory);
			const auto symbol = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);
			ASSERT_TRUE(symbol) << static_cast<int>(subcategory);
			EXPECT_EQ(symbol->Identification.Fields().SymbolSetValue, expectedSet);
			EXPECT_TRUE(Sidc::Parse(symbol->Identification.ToString()));
		}

		observation.Type = decltype(observation.Type)::Known<SubCategory>(SubCategory::Other);
		const auto unsupported = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);
		EXPECT_FALSE(unsupported);
		EXPECT_EQ(unsupported.error().Code, ConversionErrorCode::UnsupportedClassification);
	}

	TEST(DomainConverterTest, CoversEveryDeclaredCategoryAndDimensionAtItsMostSpecificAvailableLevel)
	{
		using Category = Classification::Api::Category;
		const std::array categoryCases{std::pair{Category::Aircraft, SymbolSet::Air},
									   std::pair{Category::Missile, SymbolSet::AirMissile},
									   std::pair{Category::SurfaceVessel, SymbolSet::SeaSurface},
									   std::pair{Category::Submarine, SymbolSet::SeaSubsurface},
									   std::pair{Category::Torpedo, SymbolSet::SeaSubsurface},
									   std::pair{Category::Mine, SymbolSet::MineWarfare},
									   std::pair{Category::LandEquipment, SymbolSet::LandEquipment},
									   std::pair{Category::LandInstallation, SymbolSet::LandInstallation},
									   std::pair{Category::LandUnit, SymbolSet::LandUnit},
									   std::pair{Category::SpaceObject, SymbolSet::Space},
									   std::pair{Category::SurfaceDevice, SymbolSet::SeaSurface},
									   std::pair{Category::SubsurfaceDevice, SymbolSet::SeaSubsurface},
									   std::pair{Category::SeaAnimal, SymbolSet::SeaSubsurface}};
		Observations::Api::Instance observation{};
		for (const auto& [category, expectedSet] : categoryCases)
		{
			observation.Type = decltype(observation.Type)::Known<Category>(category);
			const auto symbol = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);
			ASSERT_TRUE(symbol) << static_cast<int>(category);
			EXPECT_EQ(symbol->Identification.Fields().SymbolSetValue, expectedSet);
		}
		observation.Type = decltype(observation.Type)::Known<Category>(Category::Other);
		EXPECT_FALSE(DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown));

		using Dimension = Classification::Api::Dimension;
		const std::array dimensionCases{std::pair{Dimension::Air, SymbolSet::Air},
										std::pair{Dimension::Land, SymbolSet::LandUnit},
										std::pair{Dimension::SeaSurface, SymbolSet::SeaSurface},
										std::pair{Dimension::SeaSubsurface, SymbolSet::SeaSubsurface},
										std::pair{Dimension::Space, SymbolSet::Space}};
		for (const auto& [dimension, expectedSet] : dimensionCases)
		{
			observation.Type = decltype(observation.Type)::Known<Dimension>(dimension);
			const auto symbol = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);
			ASSERT_TRUE(symbol) << static_cast<int>(dimension);
			EXPECT_EQ(symbol->Identification.Fields().SymbolSetValue, expectedSet);
		}
		observation.Type = decltype(observation.Type)::Known<Dimension>(Dimension::Other);
		EXPECT_FALSE(DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown));

		observation.Type = decltype(observation.Type)::Known<Category>(Category::Mine);
		const auto mine = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);
		ASSERT_TRUE(mine);
		EXPECT_EQ(mine->Identification.Fields().Entity, 11);
	}

	TEST(DomainConverterTest, ResolvesExactTypeIdThroughMatchingAuthorizedTypeDefinition)
	{
		Observations::Api::Instance observation{};
		observation.Type = decltype(observation.Type)::Known<Units::Api::TypeId>(Units::Api::TypeId{"catalog-cruiser"});
		Units::Api::Type type{};
		type.Id = {"catalog-cruiser"};
		type.UnitSubCategory = Classification::Api::SubCategory::Cruiser;

		const auto symbol = DomainConverter::FromObservation(observation, type, StandardIdentity::Hostile);

		ASSERT_TRUE(symbol);
		EXPECT_EQ(symbol->Identification.Fields().SymbolSetValue, SymbolSet::SeaSurface);
		EXPECT_EQ(symbol->Identification.Fields().Identity, StandardIdentity::Hostile);
		EXPECT_EQ(symbol->ExternalAmplifiers.NameOrType, "catalog-cruiser");
	}

	TEST(DomainConverterTest, UsesOnlyStandardAssignmentsForCurrentNavyAndEquipmentClasses)
	{
		using SubCategory = Classification::Api::SubCategory;
		struct ExpectedAssignment
		{
			SubCategory Subcategory;
			SymbolSet Set;
			std::uint8_t Entity;
			std::uint8_t EntityType;
			std::uint8_t EntitySubtype;
			std::uint8_t Modifier1;
		};
		const std::array assignments{ExpectedAssignment{SubCategory::Cruiser, SymbolSet::SeaSurface, 12, 2, 2, 0},
									 ExpectedAssignment{SubCategory::Destroyer, SymbolSet::SeaSurface, 12, 2, 3, 0},
									 ExpectedAssignment{SubCategory::Frigate, SymbolSet::SeaSurface, 12, 2, 4, 0},
									 ExpectedAssignment{SubCategory::Corvette, SymbolSet::SeaSurface, 12, 2, 5, 0},
									 ExpectedAssignment{SubCategory::PatrolVessel, SymbolSet::SeaSurface, 12, 5, 0, 0},
									 ExpectedAssignment{SubCategory::AmphibiousVessel, SymbolSet::SeaSurface, 12, 3, 0, 0},
									 ExpectedAssignment{SubCategory::AuxiliaryVessel, SymbolSet::SeaSurface, 13, 1, 0, 0},
									 ExpectedAssignment{SubCategory::MerchantVessel, SymbolSet::SeaSurface, 14, 1, 0, 0},
									 ExpectedAssignment{SubCategory::AttackSubmarine, SymbolSet::SeaSubsurface, 11, 1, 0, 0},
									 ExpectedAssignment{SubCategory::BallisticMissileSubmarine, SymbolSet::SeaSubsurface, 11, 1, 0, 0},
									 ExpectedAssignment{SubCategory::CruiseMissileSubmarine, SymbolSet::SeaSubsurface, 11, 1, 0, 0},
									 ExpectedAssignment{SubCategory::NavalMine, SymbolSet::MineWarfare, 11, 0, 0, 0},
									 ExpectedAssignment{SubCategory::SurfaceToAirMissileLauncher, SymbolSet::LandEquipment, 11, 11, 0, 0},
									 ExpectedAssignment{SubCategory::AntiAircraftGun, SymbolSet::LandEquipment, 11, 5, 0, 0}};
		Observations::Api::Instance observation{};
		for (const auto& assignment : assignments)
		{
			observation.Type = decltype(observation.Type)::Known<SubCategory>(assignment.Subcategory);
			const auto symbol = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);
			ASSERT_TRUE(symbol) << static_cast<int>(assignment.Subcategory);
			const auto& fields = symbol->Identification.Fields();
			EXPECT_EQ(fields.SymbolSetValue, assignment.Set);
			EXPECT_EQ(fields.Entity, assignment.Entity);
			EXPECT_EQ(fields.EntityType, assignment.EntityType);
			EXPECT_EQ(fields.EntitySubtype, assignment.EntitySubtype);
			EXPECT_EQ(fields.Modifier1, assignment.Modifier1);
		}
	}

	TEST(DomainConverterTest, KeepsPartialPositionAndErrorEllipseWithoutInventingMissingCoordinates)
	{
		Observations::Api::Instance observation{};
		Observations::Api::PartiallyLocalizedPosition partial;
		partial.Latitude = Physics::Api::Degrees{50.0};
		partial.ErrorEllipse = Observations::Api::ObservedErrorEllipse{Physics::Api::Meters{250.0}, Physics::Api::Meters{100.0}, Physics::Api::Radians{0.4}};
		observation.Kinematics.Position = partial;

		const auto symbol = DomainConverter::FromObservation(observation, std::nullopt, StandardIdentity::Unknown);

		ASSERT_TRUE(symbol);
		ASSERT_TRUE(symbol->ExternalAmplifiers.Position);
		EXPECT_EQ(symbol->ExternalAmplifiers.Position->Location.LatitudeDegrees, 50.0);
		EXPECT_FALSE(symbol->ExternalAmplifiers.Position->Location.LongitudeDegrees.has_value());
		ASSERT_TRUE(symbol->ExternalAmplifiers.Position->Uncertainty);
		EXPECT_EQ(symbol->ExternalAmplifiers.Position->Uncertainty->SemiMajorAxisMeters, 250.0);
		EXPECT_EQ(symbol->ExternalAmplifiers.Position->Uncertainty->SemiMinorAxisMeters, 100.0);
	}
}
