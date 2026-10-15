#pragma once

#include <NavalCommand/Symbology/Nato/Api/ScreenGeometry.h>

namespace NavalCommand::Symbology::Nato::Api
{
	struct RenderResult
	{
		ScreenRectangle Bounds;
		bool WasDrawn{};
	};
}
