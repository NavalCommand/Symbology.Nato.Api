#pragma once

#include <NavalCommand/Symbology/Nato/Api/IMapProjection.h>
#include <NavalCommand/Symbology/Nato/Api/RenderLayer.h>
#include <NavalCommand/Symbology/Nato/Api/SymbolStyle.h>

namespace NavalCommand::Symbology::Nato::Api
{
	struct RenderContext
	{
		// Actual projected position: movement geometry originates here.
		ScreenPoint GeographicAnchor;
		// Caller-owned presentation position: frame, icon and attached labels.
		ScreenPoint VisualAnchor;
		ScreenRectangle Viewport;
		const IMapProjection* Projection{};
		double SymbolScale{1.0};
		double MapOrientationRadians{};
		// The speed leader length is projected distance travelled in this many seconds.
		double SpeedLeaderTimeHorizonSeconds{60.0};
		// The caller resolves selection/hover before measuring; no renderer hit testing.
		bool ShowLabels{true};
		bool DetailedLabels{true};
		SymbolStyle Style;
		RenderLayer Layer{RenderLayer::All};
	};
}
