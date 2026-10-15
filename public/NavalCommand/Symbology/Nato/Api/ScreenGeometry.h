#pragma once

namespace NavalCommand::Symbology::Nato::Api
{
	struct ScreenPoint
	{
		double X{};
		double Y{};
	};

	struct ScreenVector
	{
		double X{};
		double Y{};
	};

	struct ScreenRectangle
	{
		ScreenPoint Minimum;
		ScreenPoint Maximum;

		bool Contains(ScreenPoint point) const noexcept
		{
			return point.X >= Minimum.X && point.X <= Maximum.X && point.Y >= Minimum.Y && point.Y <= Maximum.Y;
		}
	};

}
