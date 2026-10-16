#pragma once

#include <NavalCommand/Symbology/Nato/Api/AffiliationPalette.h>
#include <NavalCommand/Symbology/Nato/Api/StylePreset.h>

namespace NavalCommand::Symbology::Nato::Api
{
	// Per-call values; no renderer state or GUI types. Unfilled symbols use the
	// affiliation colour for both frame and icon. Text always uses TextForeground.
	struct SymbolStyle
	{
		StylePreset Preset{StylePreset::FilledDark};
		AffiliationPalette Palette;
		Color IconForeground{0, 0, 0};
		Color FrameForeground{0, 0, 0};
		Color TextForeground{240, 245, 250};
		Color Halo{255, 255, 255};
		double HaloWidth{0.65};
		double LineThicknessFactor{1.0};

		static constexpr SymbolStyle FromPreset(StylePreset preset) noexcept
		{
			SymbolStyle style;
			style.Preset = preset;
			if (preset == StylePreset::FilledLight)
			{
				style.TextForeground = {0, 0, 0};
				style.HaloWidth = 0.0;
			}
			return style;
		}
	};
}
