#pragma once

#include <NavalCommand/Symbology/Nato/Api/ScreenGeometry.h>

namespace NavalCommand::Symbology::Nato::Api
{
	struct RenderResult
	{
		ScreenRectangle Bounds;
		bool WasDrawn{};
		// Frame and internal icon only; excludes labels, uncertainty and movement.
		ScreenRectangle SymbolBounds;
		ScreenRectangle LayoutBounds;
		ScreenRectangle GeographicBounds;
	};
}
