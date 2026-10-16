#pragma once

#include <optional>
#include <string>

namespace NavalCommand::Symbology::Nato::Api
{
	struct GeographicLocation
	{
		std::optional<double> LatitudeDegrees;
		std::optional<double> LongitudeDegrees;
	};

	enum class VerticalReference
	{
		Altitude,
		Depth,
		Elevation
	};

	struct VerticalPositionAmplifier
	{
		double ValueMeters{};
		VerticalReference Reference{VerticalReference::Elevation};
	};

	struct GeographicUncertaintyEllipse
	{
		double SemiMajorAxisMeters{};
		double SemiMinorAxisMeters{};
		double OrientationRadians{};
	};

	struct PositionAmplifier
	{
		GeographicLocation Location;
		std::optional<GeographicUncertaintyEllipse> Uncertainty;
	};

	struct Amplifiers
	{
		// Semantic equivalents of APP-06(E) text fields. They contain no pixels or font data.
		std::optional<std::string> UniqueDesignation;
		std::optional<std::string> NameOrType;
		std::optional<std::string> AdditionalInformation;

		// Heading remains distinct from course; only course may drive movement graphics.
		std::optional<double> HeadingRadians;
		std::optional<double> CourseOverGroundRadians;
		std::optional<double> SpeedMetersPerSecond;
		std::optional<VerticalPositionAmplifier> VerticalPosition;
		std::optional<PositionAmplifier> Position;
	};
}
