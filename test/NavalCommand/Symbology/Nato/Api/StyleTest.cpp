#include <NavalCommand/Symbology/Nato/Api/SymbolStyle.h>
#include <gtest/gtest.h>

namespace NavalCommand::Symbology::Nato::Api
{
	TEST(SymbolStyleTest, DefaultsToFilledDarkWithSharedIdentityPalette)
	{
		const SymbolStyle style;
		EXPECT_EQ(style.Preset, StylePreset::FilledDark);
		EXPECT_EQ(style.IconForeground, (Color{0, 0, 0}));
		EXPECT_EQ(style.Palette.Resolve(StandardIdentity::Friendly), (Color{128, 224, 255}));
		EXPECT_EQ(style.Palette.Resolve(StandardIdentity::Hostile), (Color{255, 128, 128}));
		EXPECT_EQ(style.Palette.Resolve(StandardIdentity::Neutral), (Color{170, 255, 170}));
		EXPECT_EQ(style.Palette.Resolve(StandardIdentity::Unknown), (Color{255, 255, 128}));
		EXPECT_EQ(style.Palette.Resolve(StandardIdentity::AssumedFriendly), style.Palette.Friendly);
		EXPECT_EQ(style.Palette.Resolve(StandardIdentity::Suspect), style.Palette.Hostile);
		EXPECT_EQ(style.Palette.Resolve(StandardIdentity::Pending), style.Palette.Unknown);
	}

	TEST(SymbolStyleTest, PresetsRemainExtensibleValues)
	{
		auto style = SymbolStyle::FromPreset(StylePreset::UnfilledDark);
		style.Palette.Friendly = {1, 2, 3};
		EXPECT_EQ(style.Palette.Resolve(StandardIdentity::Friendly), (Color{1, 2, 3}));
		EXPECT_EQ(SymbolStyle{}.Palette.Friendly, (Color{128, 224, 255}));
		EXPECT_EQ(SymbolStyle::FromPreset(StylePreset::FilledLight).TextForeground, (Color{0, 0, 0}));
	}
}
