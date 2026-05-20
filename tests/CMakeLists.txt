FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.13.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Catch2)

add_executable(openref_tests
    test_biquad_response.cpp
    test_correction_engine.cpp
    test_profile_parser.cpp
    test_fir_designer.cpp
    test_bypass_null.cpp
)

target_link_libraries(openref_tests PRIVATE openref_core Catch2::Catch2WithMain)
target_compile_features(openref_tests PRIVATE cxx_std_20)

include(CTest)
include(Catch)
catch_discover_tests(openref_tests)
