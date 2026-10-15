#pragma once

#include <NavalCommand/Symbology/Nato/Api/RenderContext.h>
#include <NavalCommand/Symbology/Nato/Api/RenderResult.h>
#include <NavalCommand/Symbology/Nato/Api/Symbol.h>

namespace NavalCommand::Symbology::Nato::Api
{
	class IRenderer
	{
	public:
		virtual ~IRenderer() = default;
		virtual RenderResult Render(const Symbol& symbol, const RenderContext& context) = 0;
	};
}
