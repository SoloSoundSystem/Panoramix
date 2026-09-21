cmake_minimum_required(VERSION 3.22)
project(Panoramix VERSION 0.1.0)

# Solo en Linux: pasar flags de FreeType/FontConfig a JUCE
if(UNIX AND NOT APPLE)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
    pkg_check_modules(FONTCONFIG REQUIRED fontconfig)
    set(_JUCE_LINUX_CXXFLAGS "${FREETYPE_CFLAGS} ${FONTCONFIG_CFLAGS}")
    if(DEFINED ENV{CXXFLAGS} AND NOT "$ENV{CXXFLAGS}" STREQUAL "")
        set(_JUCE_LINUX_CXXFLAGS "$ENV{CXXFLAGS} ${_JUCE_LINUX_CXXFLAGS}")
    endif()
    set(ENV{CXXFLAGS} "${_JUCE_LINUX_CXXFLAGS}")
endif()

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 9.0.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(Panoramix
    COMPANY_NAME "12Monos"
    BUNDLE_ID com.12monos.panoramix
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Mono
    PLUGIN_CODE Pano
    FORMATS VST3
    PRODUCT_NAME "Panoramix"
)

juce_generate_juce_header(Panoramix)

target_sources(Panoramix PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
)

target_compile_features(Panoramix PRIVATE cxx_std_17)

target_compile_definitions(Panoramix PUBLIC
    JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(Panoramix PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_processors
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)
