#pragma once

#include <cstdint>

namespace NavalCommand::Symbology::Nato::Api
{
	enum class SymbolSet : std::uint8_t
	{
		Unknown = 0,
		Air = 1,
		AirMissile = 2,
		Space = 5,
		SpaceMissile = 6,
		LandUnit = 10,
		LandCivilianUnit = 11,
		LandEquipment = 15,
		LandInstallation = 20,
		ControlMeasure = 25,
		DismountedIndividual = 27,
		SeaSurface = 30,
		SeaSubsurface = 35,
		MineWarfare = 36,
		Activities = 40,
		Atmospheric = 45,
		Oceanographic = 46,
		MeteorologicalSpace = 47,
		SignalsIntelligenceSpace = 50,
		SignalsIntelligenceAir = 51,
		SignalsIntelligenceLandEquipment = 52,
		SignalsIntelligenceSeaSurface = 53,
		SignalsIntelligenceSeaSubsurface = 54,
		Cyberspace = 60,
		CyberspaceSpace = 61,
		CyberspaceAir = 62,
		CyberspaceLandUnit = 63,
		CyberspaceLandEquipment = 64,
		CyberspaceLandInstallation = 65,
		CyberspaceDismountedIndividual = 66,
		CyberspaceSeaSurface = 67,
		CyberspaceSeaSubsurface = 68
	};
}
