cmake_minimum_required(VERSION 3.20)
project(cf4pwm_runner_win LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (MSVC)
  add_compile_definitions(
    WIN32_LEAN_AND_MEAN
    NOMINMAX
    _CRT_SECURE_NO_WARNINGS
    _USE_MATH_DEFINES
    WINDOWS_IGNORE_PACKING_MISMATCH
  )
  add_compile_options(/permissive-)
endif()

include(FetchContent)
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
  crazyflie_cpp
  GIT_REPOSITORY https://github.com/whoenig/crazyflie_cpp.git
  GIT_TAG        master
)
FetchContent_MakeAvailable(crazyflie_cpp)
find_package(libusb-1.0 CONFIG REQUIRED)

# --- MSVC: strip any -W* flags from upstream targets (both PUBLIC/INTERFACE) ---
if (MSVC)
  function(_strip_gcc_warnings tgt)
    if (TARGET ${tgt})
      foreach(prop IN ITEMS COMPILE_OPTIONS INTERFACE_COMPILE_OPTIONS)
        get_target_property(_opts ${tgt} ${prop})
        if (_opts)
          list(FILTER _opts EXCLUDE REGEX "^-W.+")
          set_property(TARGET ${tgt} PROPERTY ${prop} "${_opts}")
        endif()
      endforeach()
    endif()
  endfunction()

  _strip_gcc_warnings(crazyflie_cpp)
  _strip_gcc_warnings(crazyradio) # if present upstream, this will apply; otherwise ignored
endif()

# --- Executable ---
add_executable(cf4pwm_runner_win
  src/main_win.cpp
  src/radio_cfclient.cpp
)

# Make both <Crazyflie.h> and <crazyflie_cpp/Crazyflie.h> resolvable
set(_cf_src_dir "${crazyflie_cpp_SOURCE_DIR}")
target_include_directories(cf4pwm_runner_win PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${_cf_src_dir}/include
  ${_cf_src_dir}/include/crazyflie_cpp
)

target_link_libraries(cf4pwm_runner_win PRIVATE crazyflie_cpp libusb-1.0::libusb-1.0)
if (TARGET crazyflie_cpp)
  target_link_libraries(crazyflie_cpp PRIVATE libusb-1.0::libusb-1.0)
endif()

if (MSVC)
  target_compile_options(cf4pwm_runner_win PRIVATE /W3)
else()
  target_compile_options(cf4pwm_runner_win PRIVATE -Wall -Wextra)
endif()

if (MSVC)
  _strip_gcc_warnings(cf4pwm_runner_win)
endif()

set_target_properties(cf4pwm_runner_win PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# --- MSVC: force-include shim into our exe and upstream targets to fix ssize_t & GNU attributes ---
if (MSVC)
  set(_shim "${CMAKE_CURRENT_SOURCE_DIR}/include/cf4pwm/msvc_posix_attr_shim.hpp")
  file(TO_CMAKE_PATH "${_shim}" MSVC_SHIM)

  # our app
  target_compile_options(cf4pwm_runner_win PRIVATE /FI${MSVC_SHIM})

  # upstream libs (if the targets exist, apply the same)
  foreach(dep IN ITEMS crazyflie_cpp crazyradio)
    if (TARGET ${dep})
      target_compile_options(${dep} PRIVATE /FI${MSVC_SHIM})
    endif()
  endforeach()
endif()
