#pragma once

#include <NavalCommand/Symbology/Nato/Api/GeographicCoordinate.h>
#include <NavalCommand/Symbology/Nato/Api/ScreenGeometry.h>

namespace NavalCommand::Symbology::Nato::Api
{
	// TacticalDisplay owns the map projection. The renderer requests projected points
	// and local tangent-plane offsets so it never assumes a pixel-to-metre ratio.
	class IMapProjection
	{
	public:
		virtual ~IMapProjection() = default;
		virtual ScreenPoint Project(GeographicCoordinate coordinate) const noexcept = 0;
		virtual ScreenVector ProjectLocalOffsetMeters(
			GeographicCoordinate origin,
			double eastMeters,
			double northMeters) const noexcept = 0;
	};
}
