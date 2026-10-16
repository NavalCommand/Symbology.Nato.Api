#pragma once

#include <NavalCommand/Symbology/Nato/Api/MeasureResult.h>
#include <NavalCommand/Symbology/Nato/Api/RenderContext.h>
#include <NavalCommand/Symbology/Nato/Api/RenderResult.h>
#include <NavalCommand/Symbology/Nato/Api/Symbol.h>

namespace NavalCommand::Symbology::Nato::Api
{
	class IRenderer
	{
	public:
		virtual ~IRenderer() = default;
		// Requires the implementation's font context, but emits no draw commands.
		virtual MeasureResult Measure(const Symbol& symbol, const RenderContext& context) = 0;
		virtual RenderResult Render(const Symbol& symbol, const RenderContext& context) = 0;
	};
}
