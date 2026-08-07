cmake_minimum_required(VERSION 3.14)
project(TruePeakTest)

set(CMAKE_PREFIX_PATH "/Users/gastonschachtl/Projects/Sound_Reader/build/juce-build")
find_package(JUCE 7.0 COMPONENTS core dsp REQUIRED)

add_executable(TruePeakDetectorTest
    Source/DSP/TruePeakDetectorTest.cpp
)

target_link_libraries(TruePeakDetectorTest juce::core juce::dsp)
target_include_directories(TruePeakDetectorTest PRIVATE
    ${CMAKE_PREFIX_PATH}/JUCE/modules
)

