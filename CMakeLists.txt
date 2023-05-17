cmake_minimum_required(VERSION 3.14)

include(cmake/prelude.cmake)

project(
  PicoResponsiveAnalogRead
  VERSION 0.1.0
  DESCRIPTION "Button debouncing for the Raspberry Pi Pico SDK"
  HOMEPAGE_URL " "
  LANGUAGES NONE)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- Declare library ----

add_library(PicoResponsiveAnalogRead_PicoResponsiveAnalogRead INTERFACE)
add_library(PicoResponsiveAnalogRead::PicoResponsiveAnalogRead ALIAS
            PicoResponsiveAnalogRead_PicoResponsiveAnalogRead)

set_property(TARGET PicoResponsiveAnalogRead_PicoResponsiveAnalogRead
             PROPERTY EXPORT_NAME PicoResponsiveAnalogRead)

target_include_directories(
  PicoResponsiveAnalogRead_PicoResponsiveAnalogRead ${warning_guard}
  INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>")

target_compile_features(PicoResponsiveAnalogRead_PicoResponsiveAnalogRead
                        INTERFACE cxx_std_20)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT PicoResponsiveAnalogRead_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
    AUTHOR_WARNING
      "Developer mode is intended for developers of PicoResponsiveAnalogRead")
endif()

include(cmake/dev-mode.cmake)
