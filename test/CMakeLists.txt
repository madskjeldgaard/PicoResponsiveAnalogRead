cmake_minimum_required(VERSION 3.14)

project(PicoResponsiveAnalogReadTests LANGUAGES CXX)

include(../cmake/project-is-top-level.cmake)
include(../cmake/folders.cmake)
include(../cmake/windows-set-path.cmake)

# ---- Dependencies ----

if(PROJECT_IS_TOP_LEVEL)
  find_package(PicoResponsiveAnalogRead REQUIRED)
  enable_testing()
endif()

# ---- Tests ----

add_executable(PicoResponsiveAnalogRead_test source/PicoResponsiveAnalogRead_test.cpp)
target_link_libraries(PicoResponsiveAnalogRead_test PRIVATE PicoResponsiveAnalogRead::PicoResponsiveAnalogRead)
target_compile_features(PicoResponsiveAnalogRead_test PRIVATE cxx_std_20)

add_test(NAME PicoResponsiveAnalogRead_test COMMAND PicoResponsiveAnalogRead_test)

# ---- End-of-file commands ----

add_folders(Test)
