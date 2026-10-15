#pragma once

#include <cstdint>

namespace NavalCommand::Symbology::Nato::Api
{
	enum class HeadquartersTaskForceDummy : std::uint8_t
	{
		None = 0,
		FeintOrDummy = 1,
		Headquarters = 2,
		FeintOrDummyHeadquarters = 3,
		TaskForce = 4,
		FeintOrDummyTaskForce = 5,
		TaskForceHeadquarters = 6,
		FeintOrDummyTaskForceHeadquarters = 7
	};
}
