project_dir = "/Users/gastonschachtl/Projects/Sound_Reader"
build_dir = "build-test"
test_sources = "Source/DSP/Tests"
test_file = "TruePeakDetectorTest.cpp"

cmake_config = f"""
cmake_minimum_required(VERSION 3.14)
project(TruePeakTest LANGUAGES CXX)

if(EXISTS "{project_dir}/build/juce-src/CMakeLists.txt")
    set(JUCE_SOURCE_DIR "{project_dir}/build/juce-src")
else()
    message(FATAL_ERROR "JUCE not found. Running CMake first.")
endif()

find_package(JUCE 7.0 REQUIRED COMPONENTS core dsp)

add_executable(TruePeakDetectorTest
    {test_sources}/{test_file}
)

target_link_libraries(TruePeakDetectorTest juce::core juce::dsp)
target_include_directories(TruePeakDetectorTest PRIVATE
    ${{JUCE_MODULE_HEADERS_DIR}}
    ${project_dir}/Source/DSP
)

set_property(TARGET TruePeakDetectorTest PROPERTY CXX_STANDARD 17)
set_property(TARGET TruePeakDetectorTest PROPERTY CXX_STANDARD_REQUIRED ON)
"""

with open(f"{project_dir}/CMakeLists-test.txt", "w") as f:
    f.write(cmake_config)
print("Written CMakeLists-test.txt")