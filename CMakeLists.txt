cmake_minimum_required(VERSION 3.22)
project(NOIZZ_IMAGER VERSION 1.0.0)

# JUCE is fetched at configure time. Requires internet access on the build machine.
include(FetchContent)
FetchContent_Declare(JUCE GIT_REPOSITORY https://github.com/juce-framework/JUCE.git GIT_TAG 8.0.6 GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(NOIZZ_IMAGER
    COMPANY_NAME "NOIZZ STUDIO"
    BUNDLE_ID "studio.noizz.imager"
    PLUGIN_MANUFACTURER_CODE Nizz
    PLUGIN_CODE Nzim
    FORMATS VST3 Standalone
    PRODUCT_NAME "NOIZZ IMAGER"
    DESCRIPTION "Futuristic stereo image processor"
    IS_SYNTH FALSE NEEDS_MIDI_INPUT FALSE NEEDS_MIDI_OUTPUT FALSE IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE COPY_PLUGIN_AFTER_BUILD FALSE)

juce_generate_juce_header(NOIZZ_IMAGER)
target_sources(NOIZZ_IMAGER PRIVATE Source/PluginProcessor.cpp Source/PluginProcessor.h Source/PluginEditor.cpp Source/PluginEditor.h)
target_compile_features(NOIZZ_IMAGER PRIVATE cxx_std_17)
target_compile_definitions(NOIZZ_IMAGER PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(NOIZZ_IMAGER PRIVATE juce::juce_audio_utils juce::juce_dsp PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)
