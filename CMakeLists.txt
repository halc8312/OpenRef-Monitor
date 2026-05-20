cmake_minimum_required(VERSION 3.22)

project(OpenRefMonitor VERSION 0.1.0 LANGUAGES C CXX)

option(OPENREF_BUILD_PLUGIN "Build the JUCE VST3/standalone plugin" ON)
option(OPENREF_BUILD_TESTS "Build unit tests" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(nlohmann_json)

add_library(openref_core
    src/dsp/Biquad.cpp
    src/dsp/BiquadDesigner.cpp
    src/dsp/CorrectionEngine.cpp
    src/dsp/FirDesigner.cpp
    src/dsp/ConvolutionEngine.cpp
    src/dsp/ResponseAnalyzer.cpp
    src/dsp/Metering.cpp
    src/dsp/SmoothedBypass.cpp
    src/profile/CalibrationProfile.cpp
    src/profile/ProfileManager.cpp
    src/profile/ProfileJson.cpp
)

set_target_properties(openref_core PROPERTIES POSITION_INDEPENDENT_CODE ON)
target_include_directories(openref_core PUBLIC src)
target_link_libraries(openref_core PUBLIC nlohmann_json::nlohmann_json)
target_compile_features(openref_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(openref_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(openref_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(OPENREF_BUILD_PLUGIN)
    FetchContent_Declare(
        JUCE
        GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
        GIT_TAG 8.0.12
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(JUCE)

    juce_add_binary_data(OpenRefAssets
        SOURCES
            resources/profiles/audio-technica-ath-m70x.autoeq.json
            resources/targets/flat_custom.json
            resources/targets/neutral_harman_like.json
            resources/simulations/phone_small_speaker.json
            resources/simulations/laptop_speaker.json
            resources/simulations/car_basic.json
            resources/simulations/mono_midrange.json
            resources/simulations/bass_limited_check.json
            resources/simulations/bright_earbuds_check.json
    )

    juce_add_plugin(OpenRefMonitor
        COMPANY_NAME "OpenRef Project"
        PRODUCT_NAME "OpenRef Monitor"
        BUNDLE_ID "org.openref.monitor"
        PLUGIN_MANUFACTURER_CODE Orfm
        PLUGIN_CODE Orm1
        FORMATS VST3 Standalone
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT FALSE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD FALSE
        VST3_CATEGORIES Fx Analyzer
    )

    target_sources(OpenRefMonitor PRIVATE
        src/PluginProcessor.cpp
        src/PluginEditor.cpp
        src/Parameters.cpp
        src/ui/ResponseGraph.cpp
        src/ui/CurveEditor.cpp
        src/ui/MeterView.cpp
        src/ui/ProfileSelector.cpp
    )

    target_link_libraries(OpenRefMonitor
        PRIVATE
            openref_core
            OpenRefAssets
            juce::juce_audio_utils
            juce::juce_dsp
        PUBLIC
            juce::juce_recommended_config_flags
            juce::juce_recommended_lto_flags
            juce::juce_recommended_warning_flags
    )

    target_compile_definitions(OpenRefMonitor
        PUBLIC
            JUCE_WEB_BROWSER=0
            JUCE_USE_CURL=0
            JUCE_VST3_CAN_REPLACE_VST2=0
            OPENREF_MONITOR_VERSION="${PROJECT_VERSION}"
    )
endif()

if(OPENREF_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
