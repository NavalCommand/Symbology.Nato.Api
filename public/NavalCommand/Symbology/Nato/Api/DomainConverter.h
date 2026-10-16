#pragma once

#include <NavalCommand/Classification/Api/Alignment.h>
#include <NavalCommand/Observations/Api/Instance.h>
#include <NavalCommand/Symbology/Nato/Api/Symbol.h>
#include <NavalCommand/Units/Api/Instance.h>
#include <NavalCommand/Units/Api/Type.h>
#include <expected>
#include <optional>
#include <string>

namespace NavalCommand::Symbology::Nato::Api
{
	enum class ConversionErrorCode
	{
		UnsupportedClassification,
		InvalidDomainValue,
		MismatchedTypeDefinition,
		InvalidSidc
	};

	struct ConversionError
	{
		ConversionErrorCode Code;
		std::string Message;
	};

	// Domain adapters are additive. Sidc and Symbol remain useful without Domain.Api.
	class DomainConverter final
	{
	public:
		static StandardIdentity FromAlignment(std::optional<Classification::Api::Alignment> alignment) noexcept;

		static std::expected<Symbol, ConversionError> FromUnit(
			const Units::Api::Instance& unit,
			const std::optional<Units::Api::Type>& authorizedTypeDefinition,
			StandardIdentity identity,
			Context context = Context::Reality);

		static std::expected<Symbol, ConversionError> FromObservation(
			const Observations::Api::Instance& observation,
			const std::optional<Units::Api::Type>& authorizedTypeDefinition,
			StandardIdentity identity,
			Context context = Context::Reality);
	};
}
