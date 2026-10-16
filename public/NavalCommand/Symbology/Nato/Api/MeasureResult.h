#pragma once

#include <NavalCommand/Symbology/Nato/Api/ScreenGeometry.h>

namespace NavalCommand::Symbology::Nato::Api
{
	struct MeasureResult
	{
		ScreenRectangle Bounds;
		ScreenRectangle SymbolBounds;
		// Movable frame, modifiers and visible labels; collision layout uses this.
		ScreenRectangle LayoutBounds;
		// Fixed movement and uncertainty. Degenerate at GeographicAnchor if absent.
		ScreenRectangle GeographicBounds;
		bool Supported{};
	};
}
