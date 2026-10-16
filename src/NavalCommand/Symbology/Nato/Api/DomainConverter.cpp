#include <NavalCommand/Observations/Api/PartiallyLocalizedPosition.h>
#include <NavalCommand/Observations/Api/Position.h>
#include <NavalCommand/Physics/Api/Position.h>
#include <NavalCommand/Symbology/Nato/Api/DomainConverter.h>
#include <NavalCommand/Units/Api/TypeId.h>
#include <algorithm>
#include <utility>
#include <variant>

namespace NavalCommand::Symbology::Nato::Api
{
	namespace
	{
		using Classification::Api::Category;
		using Classification::Api::Dimension;
		using Classification::Api::SubCategory;

		struct ClassificationCode
		{
			SymbolSet Set{SymbolSet::Unknown};
			std::uint8_t Entity{};
			std::uint8_t EntityType{};
			std::uint8_t EntitySubtype{};
			std::uint8_t Modifier1{};
			std::optional<std::string> Name;
			std::uint8_t Modifier2{};
		};

		ConversionError Error(ConversionErrorCode code, std::string message)
		{
			return {code, std::move(message)};
		}

		std::string TypeLabel(const Units::Api::TypeId& typeId, const std::optional<Units::Api::Type>& definition)
		{
			if (!definition || definition->Id != typeId)
			{
				return typeId.Value;
			}
			if (!definition->DisplayName)
			{
				return definition->AlternativeDisplayName.value_or(typeId.Value);
			}
			std::string label = *definition->DisplayName;
			if (definition->AlternativeDisplayName)
			{
				label += " (" + *definition->AlternativeDisplayName + ")";
			}
			return label;
		}

		std::expected<ClassificationCode, ConversionError> ForSubCategory(SubCategory subCategory)
		{
			switch (subCategory)
			{
			case SubCategory::FixedWingAircraft:
				return ClassificationCode{SymbolSet::Air, 11, 1};
			case SubCategory::RotaryWingAircraft:
				return ClassificationCode{SymbolSet::Air, 11, 2};
			case SubCategory::TiltRotorAircraft:
				return ClassificationCode{SymbolSet::Air, 0, 0, 0, 0, "Tilt-rotor aircraft"};
			case SubCategory::CruiseMissile:
				return ClassificationCode{SymbolSet::AirMissile, 11, 0, 0, 7, "Cruise missile"};
			case SubCategory::BallisticMissile:
				return ClassificationCode{SymbolSet::AirMissile, 11, 0, 0, 6, "Ballistic missile"};
			case SubCategory::Missile:
				return ClassificationCode{SymbolSet::AirMissile, 11, 0, 0, 0, "Missile"};
			case SubCategory::AircraftCarrier:
				return ClassificationCode{SymbolSet::SeaSurface, 12, 1, 0, 0, "Aircraft carrier"};
			case SubCategory::Cruiser:
				return ClassificationCode{SymbolSet::SeaSurface, 12, 2, 2, 0, "Cruiser"};
			case SubCategory::Destroyer:
				return ClassificationCode{SymbolSet::SeaSurface, 12, 2, 3, 0, "Destroyer"};
			case SubCategory::Frigate:
				return ClassificationCode{SymbolSet::SeaSurface, 12, 2, 4, 0, "Frigate"};
			case SubCategory::Corvette:
				return ClassificationCode{SymbolSet::SeaSurface, 12, 2, 5, 0, "Corvette"};
			case SubCategory::PatrolVessel:
				return ClassificationCode{SymbolSet::SeaSurface, 12, 5, 0, 0, "Patrol"};
			case SubCategory::AmphibiousVessel:
				return ClassificationCode{SymbolSet::SeaSurface, 12, 3, 0, 0, "Amphibious warfare ship"};
			case SubCategory::AuxiliaryVessel:
				return ClassificationCode{SymbolSet::SeaSurface, 13, 1, 0, 0, "Auxiliary ship"};
			case SubCategory::MerchantVessel:
				return ClassificationCode{SymbolSet::SeaSurface, 14, 1, 0, 0, "Merchant ship"};
			case SubCategory::AttackSubmarine:
				return ClassificationCode{SymbolSet::SeaSubsurface, 11, 1, 0, 8, "Submarine"};
			case SubCategory::BallisticMissileSubmarine:
				return ClassificationCode{SymbolSet::SeaSubsurface, 11, 1, 0, 9, "Submarine"};
			case SubCategory::CruiseMissileSubmarine:
				return ClassificationCode{SymbolSet::SeaSubsurface, 11, 1, 0, 10, "Submarine"};
			case SubCategory::Submarine:
				return ClassificationCode{SymbolSet::SeaSubsurface, 11, 1, 0, 0, "Submarine"};
			case SubCategory::HeavyweightTorpedo:
			case SubCategory::LightweightTorpedo:
			case SubCategory::Torpedo:
				return ClassificationCode{SymbolSet::SeaSubsurface, 13, 1, 0, 0, "Torpedo"};
			case SubCategory::Noisemaker:
			case SubCategory::SelfPropelledAcousticDecoy:
				return ClassificationCode{SymbolSet::SeaSubsurface, 13, 3, 0, 0, "Acoustic decoy"};
			case SubCategory::Sonobuoy:
				return ClassificationCode{SymbolSet::SeaSurface, 0, 0, 0, 0, "Sonobuoy"};
			case SubCategory::NavalMine:
				return ClassificationCode{SymbolSet::MineWarfare, 11, 0, 0, 0, "Naval mine"};
			case SubCategory::GroundVehicle:
				return ClassificationCode{SymbolSet::LandEquipment, 0, 0, 0, 0, "Vehicle"};
			case SubCategory::SurfaceToAirMissileLauncher:
				return ClassificationCode{SymbolSet::LandEquipment, 11, 11, 0, 0, "Surface-to-air missile launcher"};
			case SubCategory::AntiAircraftGun:
				return ClassificationCode{SymbolSet::LandEquipment, 11, 5, 0, 0, "Air defence gun"};
			case SubCategory::Radar:
				return ClassificationCode{SymbolSet::LandEquipment, 0, 0, 0, 0, std::nullopt};
			case SubCategory::Airfield:
				return ClassificationCode{SymbolSet::LandInstallation, 0, 0, 0, 0, "Airfield"};
			case SubCategory::MilitaryInstallation:
				return ClassificationCode{SymbolSet::LandInstallation, 11, 0, 0, 0, "Military installation"};
			case SubCategory::CivilianInstallation:
				return ClassificationCode{SymbolSet::LandInstallation, 0, 0, 0, 0, std::nullopt};
			case SubCategory::Whale:
				return ClassificationCode{SymbolSet::SeaSubsurface, 0, 0, 0, 0, "Sea animal"};
			case SubCategory::SurfaceVessel:
				return ClassificationCode{SymbolSet::SeaSurface, 0, 0, 0, 0, "Surface vessel"};
			case SubCategory::Other:
				return std::unexpected(Error(ConversionErrorCode::UnsupportedClassification, "APP-06(E)(1) has no safe generic mapping for the known Other subcategory."));
			}
			return std::unexpected(Error(ConversionErrorCode::InvalidDomainValue, "Domain subcategory is not a declared classification value."));
		}

		std::expected<ClassificationCode, ConversionError> ForCategory(Category category)
		{
			switch (category)
			{
			case Category::Aircraft:
				return ClassificationCode{SymbolSet::Air};
			case Category::Missile:
				return ClassificationCode{SymbolSet::AirMissile, 11, 0, 0, 0, "Missile"};
			case Category::SurfaceVessel:
				return ClassificationCode{SymbolSet::SeaSurface, 0, 0, 0, 0, "Surface vessel"};
			case Category::Submarine:
				return ClassificationCode{SymbolSet::SeaSubsurface, 11, 1, 0, 0, "Submarine"};
			case Category::Torpedo:
				return ClassificationCode{SymbolSet::SeaSubsurface, 13, 1, 0, 0, "Torpedo"};
			case Category::Mine:
				return ClassificationCode{SymbolSet::MineWarfare, 11, 0, 0, 0, "Mine"};
			case Category::LandEquipment:
				return ClassificationCode{SymbolSet::LandEquipment, 0, 0, 0, 0, "Land equipment"};
			case Category::LandInstallation:
				return ClassificationCode{SymbolSet::LandInstallation, 0, 0, 0, 0, "Land installation"};
			case Category::LandUnit:
				return ClassificationCode{SymbolSet::LandUnit, 0, 0, 0, 0, "Land unit"};
			case Category::SpaceObject:
				return ClassificationCode{SymbolSet::Space, 0, 0, 0, 0, "Space object"};
			case Category::SurfaceDevice:
				return ClassificationCode{SymbolSet::SeaSurface, 0, 0, 0, 0, "Surface device"};
			case Category::SubsurfaceDevice:
				return ClassificationCode{SymbolSet::SeaSubsurface, 0, 0, 0, 0, "Subsurface device"};
			case Category::SeaAnimal:
				return ClassificationCode{SymbolSet::SeaSubsurface, 0, 0, 0, 0, "Sea animal"};
			case Category::Other:
				return std::unexpected(Error(ConversionErrorCode::UnsupportedClassification, "APP-06(E)(1) has no safe generic mapping for the known Other category."));
			}
			return std::unexpected(Error(ConversionErrorCode::InvalidDomainValue, "Domain category is not a declared classification value."));
		}

		std::expected<ClassificationCode, ConversionError> ForDimension(Dimension dimension)
		{
			switch (dimension)
			{
			case Dimension::Air:
				return ClassificationCode{SymbolSet::Air};
			case Dimension::Land:
				return ClassificationCode{SymbolSet::LandUnit};
			case Dimension::SeaSurface:
				return ClassificationCode{SymbolSet::SeaSurface};
			case Dimension::SeaSubsurface:
				return ClassificationCode{SymbolSet::SeaSubsurface};
			case Dimension::Space:
				return ClassificationCode{SymbolSet::Space};
			case Dimension::Other:
				return std::unexpected(Error(ConversionErrorCode::UnsupportedClassification, "The known Other dimension has no APP-06(E)(1) dimension symbol set."));
			}
			return std::unexpected(Error(ConversionErrorCode::InvalidDomainValue, "Domain dimension is not a declared classification value."));
		}

		std::expected<ClassificationCode, ConversionError> ForType(const Units::Api::Type& type)
		{
			auto classification = ForSubCategory(type.UnitSubCategory);
			if (!classification)
			{
				return classification;
			}
			if (type.UnitSubCategory == SubCategory::FixedWingAircraft || type.UnitSubCategory == SubCategory::RotaryWingAircraft)
			{
				using Classification::Api::Role;
				if (type.PrimaryPlatformRole)
				{
					switch (*type.PrimaryPlatformRole)
					{
					case Role::AirSuperiority:
						classification->Modifier1 = 4;
						break;
					case Role::AirborneEarlyWarning:
						classification->Modifier1 = 12;
						break;
					case Role::ElectronicWarfare:
						classification->Modifier1 = 16;
						break;
					case Role::Strike:
					case Role::LandAttack:
						classification->Modifier1 = 1;
						break;
					case Role::AntiSubmarineWarfare:
						classification->Modifier1 = 2;
						break;
					case Role::Transport:
						classification->Modifier1 = 3;
						break;
					case Role::AerialRefuelling:
						classification->Modifier1 = 5;
						break;
					case Role::MaritimePatrol:
						classification->Modifier1 = 17;
						break;
					case Role::Reconnaissance:
						classification->Modifier1 = 18;
						break;
					default:
						break;
					}
				}
			}
			if (type.UnitSubCategory == SubCategory::Missile && type.PrimaryPlatformRole == Classification::Api::Role::AirSuperiority)
			{
				classification->Modifier1 = 1; // Air launch origin.
				classification->Modifier2 = 1; // Air destination.
			}
			return classification;
		}

		void CopyPosition(const Units::Api::Kinematics& kinematics, Amplifiers& amplifiers)
		{
			if (kinematics.Position)
			{
				PositionAmplifier position;
				position.Location.LatitudeDegrees = kinematics.Position->Latitude.Value;
				position.Location.LongitudeDegrees = kinematics.Position->Longitude.Value;
				position.Uncertainty = std::nullopt;
				amplifiers.Position = std::move(position);
				amplifiers.VerticalPosition = VerticalPositionAmplifier{kinematics.Position->Elevation.Value, VerticalReference::Elevation};
			}
			if (kinematics.Heading)
			{
				amplifiers.HeadingRadians = kinematics.Heading->Value;
			}
			if (kinematics.CourseOverGround)
			{
				amplifiers.CourseOverGroundRadians = kinematics.CourseOverGround->Value;
			}
			if (kinematics.Speed)
			{
				amplifiers.SpeedMetersPerSecond = kinematics.Speed->Value;
			}
		}

		void CopyPosition(const Observations::Api::Kinematics& kinematics, Amplifiers& amplifiers)
		{
			PositionAmplifier position;
			if (const auto* exact = std::get_if<Physics::Api::Position>(&kinematics.Position))
			{
				position.Location.LatitudeDegrees = exact->Latitude.Value;
				position.Location.LongitudeDegrees = exact->Longitude.Value;
				amplifiers.VerticalPosition = VerticalPositionAmplifier{exact->Elevation.Value, VerticalReference::Elevation};
			}
			else if (const auto* partial = std::get_if<Observations::Api::PartiallyLocalizedPosition>(&kinematics.Position))
			{
				if (partial->Latitude)
				{
					position.Location.LatitudeDegrees = partial->Latitude->Value;
				}
				if (partial->Longitude)
				{
					position.Location.LongitudeDegrees = partial->Longitude->Value;
				}
				if (partial->Elevation)
				{
					amplifiers.VerticalPosition = VerticalPositionAmplifier{partial->Elevation->Value, VerticalReference::Elevation};
				}
				if (partial->ErrorEllipse)
				{
					position.Uncertainty =
						GeographicUncertaintyEllipse{partial->ErrorEllipse->MajorAxis.Value, partial->ErrorEllipse->MinorAxis.Value, partial->ErrorEllipse->Orientation.Value};
				}
			}
			if (position.Location.LatitudeDegrees || position.Location.LongitudeDegrees || position.Uncertainty)
			{
				amplifiers.Position = std::move(position);
			}
			if (kinematics.Heading.IsKnown())
			{
				amplifiers.HeadingRadians = kinematics.Heading.Value().Value;
			}
			if (kinematics.CourseOverGround.IsKnown())
			{
				amplifiers.CourseOverGroundRadians = kinematics.CourseOverGround.Value().Value;
			}
			if (kinematics.Speed.IsKnown())
			{
				amplifiers.SpeedMetersPerSecond = kinematics.Speed.Value().Value;
			}
		}

		std::expected<Symbol, ConversionError> MakeSymbol(
			const ClassificationCode& classification, StandardIdentity identity, SymbolStatus status, Amplifiers amplifiers, Context context, std::optional<bool> isDestroyed)
		{
			SidcFields fields;
			fields.ContextValue = context;
			fields.Identity = identity;
			fields.SymbolSetValue = classification.Set;
			fields.Status = status;
			fields.Entity = classification.Entity;
			fields.EntityType = classification.EntityType;
			fields.EntitySubtype = classification.EntitySubtype;
			fields.Modifier1 = classification.Modifier1;
			fields.Modifier2 = classification.Modifier2;
			auto sidc = Sidc::Create(std::move(fields));
			if (!sidc)
			{
				return std::unexpected(Error(ConversionErrorCode::InvalidSidc, sidc.error().Message));
			}
			if (classification.Name && !amplifiers.NameOrType)
			{
				amplifiers.NameOrType = classification.Name;
			}
			return Symbol{std::move(*sidc), std::move(amplifiers), isDestroyed};
		}

		std::expected<ClassificationCode, ConversionError> ObservationClassification(const Observations::Api::Instance& observation,
																					 const std::optional<Units::Api::Type>& authorizedTypeDefinition)
		{
			if (const auto* exactTypeId = observation.Type.GetIf<Units::Api::TypeId>())
			{
				if (!authorizedTypeDefinition || authorizedTypeDefinition->Id != *exactTypeId)
				{
					// An exact identifier does not disclose the catalog record's subcategory.
					return ClassificationCode{SymbolSet::Unknown};
				}
				return ForType(*authorizedTypeDefinition);
			}
			if (const auto* subCategory = observation.Type.GetIf<SubCategory>())
			{
				return ForSubCategory(*subCategory);
			}
			if (const auto* category = observation.Type.GetIf<Category>())
			{
				return ForCategory(*category);
			}
			if (const auto* dimension = observation.Type.GetIf<Dimension>())
			{
				return ForDimension(*dimension);
			}
			return ClassificationCode{SymbolSet::Unknown, 0, 0, 0, 0, "Unknown classification"};
		}
	}

	StandardIdentity DomainConverter::FromAlignment(std::optional<Classification::Api::Alignment> alignment) noexcept
	{
		if (!alignment)
		{
			return StandardIdentity::Unknown;
		}
		switch (*alignment)
		{
		case Classification::Api::Alignment::Friendly:
			return StandardIdentity::Friendly;
		case Classification::Api::Alignment::Neutral:
			return StandardIdentity::Neutral;
		case Classification::Api::Alignment::Hostile:
			return StandardIdentity::Hostile;
		}
		return StandardIdentity::Unknown;
	}

	std::expected<Symbol, ConversionError>
	DomainConverter::FromUnit(const Units::Api::Instance& unit, const std::optional<Units::Api::Type>& authorizedTypeDefinition, StandardIdentity identity, Context context)
	{
		// A TypeId identifies the domain type; its matching catalog record supplies the subcategory.
		ClassificationCode classification;
		if (authorizedTypeDefinition)
		{
			if (authorizedTypeDefinition->Id != unit.Type)
			{
				return std::unexpected(Error(ConversionErrorCode::MismatchedTypeDefinition, "The supplied unit type definition does not match the unit's TypeId."));
			}
			auto mapped = ForType(*authorizedTypeDefinition);
			if (!mapped)
			{
				return std::unexpected(mapped.error());
			}
			classification = std::move(*mapped);
		}

		Amplifiers amplifiers;
		amplifiers.UniqueDesignation = unit.Name.empty() ? unit.Identity.Value : unit.Name;
		amplifiers.NameOrType = TypeLabel(unit.Type, authorizedTypeDefinition);
		CopyPosition(unit.Kinematics, amplifiers);
		return MakeSymbol(classification, identity, unit.IsDestroyed ? SymbolStatus::PresentDestroyed : SymbolStatus::Present, std::move(amplifiers), context, unit.IsDestroyed);
	}

	std::expected<Symbol, ConversionError> DomainConverter::FromObservation(const Observations::Api::Instance& observation,
																			const std::optional<Units::Api::Type>& authorizedTypeDefinition,
																			StandardIdentity identity,
																			Context context)
	{
		auto classification = ObservationClassification(observation, authorizedTypeDefinition);
		if (!classification)
		{
			return std::unexpected(classification.error());
		}

		Amplifiers amplifiers;
		amplifiers.UniqueDesignation = observation.ObservationName.empty() ? observation.Identity.Value : observation.ObservationName;
		if (const auto* exactTypeId = observation.Type.GetIf<Units::Api::TypeId>())
		{
			amplifiers.NameOrType = TypeLabel(*exactTypeId, authorizedTypeDefinition);
		}
		else if (!classification->Name)
		{
			classification->Name = "Observer-resolved classification";
		}
		CopyPosition(observation.Kinematics, amplifiers);
		const std::optional<bool> isDestroyed = observation.IsDestroyed.IsKnown() ? std::optional{observation.IsDestroyed.Value()} : std::nullopt;
		const auto status = isDestroyed.value_or(false) ? SymbolStatus::PresentDestroyed : SymbolStatus::Present;
		return MakeSymbol(*classification, identity, status, std::move(amplifiers), context, isDestroyed);
	}
}
