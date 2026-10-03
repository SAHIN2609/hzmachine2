cmake_minimum_required(VERSION 3.25)

project(SISHHIN_HZ_MACHINE VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.8
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(SISHHIN_HZ_MACHINE
    COMPANY_NAME "SISHHIN"
    PRODUCT_NAME "SISHHIN HZ MACHINE"
    BUNDLE_ID "com.sisshin.hzmachine"
    PLUGIN_MANUFACTURER_CODE Sshn
    PLUGIN_CODE HzMa
    FORMATS VST3
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
)

target_sources(SISHHIN_HZ_MACHINE PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
)

target_compile_definitions(SISHHIN_HZ_MACHINE PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
)

target_link_libraries(SISHHIN_HZ_MACHINE PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
)

juce_add_binary_data(SISHHINAssets
    SOURCES
        Assets/index.html
)

target_link_libraries(SISHHIN_HZ_MACHINE PRIVATE SISHHINAssets)
