find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_executable(NavalCommand.Symbology.Nato.Api.Test
    NavalCommand/Symbology/Nato/Api/SidcTest.cpp
    NavalCommand/Symbology/Nato/Api/StyleTest.cpp)
target_link_libraries(NavalCommand.Symbology.Nato.Api.Test PRIVATE NavalCommand.Symbology.Nato.Api GTest::gtest_main)
NavalCommandConfigureTarget(NavalCommand.Symbology.Nato.Api.Test)
gtest_discover_tests(NavalCommand.Symbology.Nato.Api.Test DISCOVERY_MODE PRE_TEST)

add_executable(NavalCommand.Symbology.Nato.Api.Domain.Test
    NavalCommand/Symbology/Nato/Api/DomainConverterTest.cpp)
target_link_libraries(NavalCommand.Symbology.Nato.Api.Domain.Test PRIVATE NavalCommand.Symbology.Nato.Api.Domain GTest::gtest_main)
NavalCommandConfigureTarget(NavalCommand.Symbology.Nato.Api.Domain.Test)
gtest_discover_tests(NavalCommand.Symbology.Nato.Api.Domain.Test DISCOVERY_MODE PRE_TEST)
