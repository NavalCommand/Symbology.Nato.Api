#pragma once

#include <NavalCommand/Symbology/Nato/Api/IMapProjection.h>

namespace NavalCommand::Symbology::Nato::Api
{
	struct RenderContext
	{
		ScreenPoint Anchor;
		ScreenRectangle Viewport;
		const IMapProjection* Projection{};
		double SymbolScale{1.0};
		double MapOrientationRadians{};
		// The speed leader length is projected distance travelled in this many seconds.
		double SpeedLeaderTimeHorizonSeconds{60.0};
	};
}
