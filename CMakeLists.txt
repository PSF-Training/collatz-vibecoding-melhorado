cmake_minimum_required(VERSION 3.21)

option(COLLATZ_BUILD_GUI "Build the wxWidgets GUI target" ON)
option(COLLATZ_BUILD_TESTS "Build the Catch2 test target" ON)

# Requesting the vcpkg feature here keeps the whole GTK/X11 tree out of the
# dependency solve when the GUI is off, so core, CLI and tests stay lightweight.
# It must be set before project(), which is where the manifest is installed.
if(COLLATZ_BUILD_GUI)
  list(APPEND VCPKG_MANIFEST_FEATURES "gui")
endif()

# The project is consumed in vcpkg manifest mode; the toolchain file that installs
# vcpkg.json dependencies is supplied by the CMake presets.
project(collatz_vibecoding_melhorado
        VERSION 0.1.0
        DESCRIPTION "Presents Collatz sequences for a series of input numbers"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CLI11 CONFIG REQUIRED)

# The core engine stays a separable library so the CLI, the GUI and the tests all
# share one implementation of the Collatz rules.
add_library(collatz_core src/collatz.cpp)
target_include_directories(collatz_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(collatz_core PUBLIC cxx_std_20)

# Warnings are enabled strictly because the overflow guard is numeric and a silent
# narrowing or sign mistake there would defeat the whole safety requirement.
if(MSVC)
  target_compile_options(collatz_core PRIVATE /W4)
else()
  target_compile_options(collatz_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(collatz src/main_cli.cpp)
target_link_libraries(collatz PRIVATE collatz_core CLI11::CLI11)

if(COLLATZ_BUILD_GUI)
  find_package(wxWidgets CONFIG REQUIRED)
  add_executable(collatz-gui src/main_gui.cpp)
  # wxWidgets 3.3 exports namespaced imported targets; older configs only expose
  # the legacy variables, so both shapes are supported.
  if(TARGET wx::core)
    target_link_libraries(collatz-gui PRIVATE collatz_core wx::core wx::base)
  else()
    include(${wxWidgets_USE_FILE})
    target_link_libraries(collatz-gui PRIVATE collatz_core ${wxWidgets_LIBRARIES})
  endif()
endif()

if(COLLATZ_BUILD_TESTS)
  include(CTest)
  find_package(Catch2 3 CONFIG REQUIRED)
  add_executable(collatz_tests tests/test_collatz.cpp)
  target_link_libraries(collatz_tests PRIVATE collatz_core Catch2::Catch2WithMain)
  add_test(NAME collatz_tests COMMAND collatz_tests)
endif()
