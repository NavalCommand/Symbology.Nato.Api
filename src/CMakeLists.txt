add_library(NavalCommand.Symbology.Nato.Api STATIC
    NavalCommand/Symbology/Nato/Api/Sidc.cpp
    NavalCommand/Symbology/Nato/Api/SymbolConverter.cpp)
target_include_directories(NavalCommand.Symbology.Nato.Api PUBLIC "${PROJECT_SOURCE_DIR}/public")
NavalCommandConfigureTarget(NavalCommand.Symbology.Nato.Api)

add_library(NavalCommand.Symbology.Nato.Api.Domain STATIC
    NavalCommand/Symbology/Nato/Api/DomainConverter.cpp)
target_include_directories(NavalCommand.Symbology.Nato.Api.Domain PUBLIC "${PROJECT_SOURCE_DIR}/public")
target_link_libraries(NavalCommand.Symbology.Nato.Api.Domain PUBLIC
    NavalCommand.Symbology.Nato.Api
    NavalCommand.Units.Api
    NavalCommand.Observations.Api)
NavalCommandConfigureTarget(NavalCommand.Symbology.Nato.Api.Domain)
