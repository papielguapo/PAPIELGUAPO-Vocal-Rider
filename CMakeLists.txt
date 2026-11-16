cmake_minimum_required(VERSION 3.22)
project(Rider VERSION 1.3.3 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
option(Rider_COPY_PLUGIN_AFTER_BUILD "Install VST3 after building (may require elevation)" OFF)
set(Rider_JUCE_PATH "C:/JUCE" CACHE PATH "Optional JUCE 7/8/9 source checkout")
if(EXISTS "${Rider_JUCE_PATH}/CMakeLists.txt")
    file(STRINGS "${Rider_JUCE_PATH}/CMakeLists.txt" _juce_version REGEX "project\\(JUCE VERSION [789]\\.")
endif()
if(_juce_version)
    add_subdirectory("${Rider_JUCE_PATH}" "${CMAKE_BINARY_DIR}/JUCE")
else()
    include(FetchContent)
    FetchContent_Declare(JUCE URL https://github.com/juce-framework/JUCE/archive/refs/tags/9.0.5.tar.gz DOWNLOAD_EXTRACT_TIMESTAMP TRUE)
    FetchContent_MakeAvailable(JUCE)
endif()
juce_add_plugin(Rider
    COMPANY_NAME "Papielguapo"
    BUNDLE_ID "com.papielguapo.vocalriderk"
    IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD ${Rider_COPY_PLUGIN_AFTER_BUILD}
    PLUGIN_MANUFACTURER_CODE Papi PLUGIN_CODE VRk1
    FORMATS VST3 Standalone PRODUCT_NAME "PAPIELGUAPO - Vocal Rider")
target_sources(Rider PRIVATE Source/PluginProcessor.h Source/PluginProcessor.cpp Source/PluginEditor.h Source/PluginEditor.cpp)
target_compile_definitions(Rider PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(Rider PRIVATE juce::juce_audio_utils PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_warning_flags)
option(Rider_BUILD_TESTS "Build DSP verification" ON)
if(Rider_BUILD_TESTS)
 enable_testing()
 juce_add_console_app(RiderTests PRODUCT_NAME "RiderTests")
 target_sources(RiderTests PRIVATE Tests/Tests.cpp Source/PluginProcessor.cpp Source/PluginEditor.cpp)
 target_compile_definitions(RiderTests PRIVATE JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
 target_link_libraries(RiderTests PRIVATE juce::juce_audio_utils juce::juce_recommended_config_flags)
 add_test(NAME DSP COMMAND RiderTests)
endif()
