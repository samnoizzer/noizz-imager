cmake_minimum_required(VERSION 3.22)
project(NoizzImager VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Automatically download JUCE from GitHub during the cloud build
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(JUCE)

# Define the VST3 and Standalone Plugin Target
juce_add_plugin(NoizzImager
    COMPANY_NAME "NOIZZ STUDIO"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Noiz
    PLUGIN_CODE NzIm
    FORMATS VST3 Standalone
    PRODUCT_NAME "NOIZZ IMAGER"
)

# Attach your C++ source files
target_sources(NoizzImager
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
)

# Embed Source/logo.png into C++ BinaryData::logo_png
juce_add_binary_data(NoizzImagerData
    SOURCES
        Source/logo.png
)

target_compile_definitions(NoizzImager
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(NoizzImager
    PRIVATE
        NoizzImagerData
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)
