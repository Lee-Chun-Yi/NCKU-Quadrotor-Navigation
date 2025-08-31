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

# ---------- vcpkg: libusb ----------
if(NOT DEFINED VCPKG_ROOT)
  if(DEFINED ENV{VCPKG_ROOT})
    set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")
  else()
    set(VCPKG_ROOT "C:/vcpkg")
  endif()
endif()
message(STATUS ">>> Using VCPKG_ROOT='${VCPKG_ROOT}'")

set(_VCPKG_INC "${VCPKG_ROOT}/installed/x64-windows/include")
set(_VCPKG_LIB "${VCPKG_ROOT}/installed/x64-windows/lib")

if(EXISTS "${_VCPKG_INC}/libusb-1.0/libusb.h")
  set(LIBUSB_INCLUDE_DIR "${_VCPKG_INC}" CACHE PATH "libusb include dir" FORCE)
else()
  message(FATAL_ERROR "libusb.h not found under ${_VCPKG_INC}/libusb-1.0")
endif()

if(EXISTS "${_VCPKG_LIB}/libusb-1.0.lib")
  set(LIBUSB_LIBRARY "${_VCPKG_LIB}/libusb-1.0.lib" CACHE FILEPATH "libusb import lib" FORCE)
else()
  message(FATAL_ERROR "libusb-1.0.lib not found under ${_VCPKG_LIB}")
endif()

message(STATUS "Found libusb include: ${LIBUSB_INCLUDE_DIR}")
message(STATUS "Found libusb lib:     ${LIBUSB_LIBRARY}")

# Provide common USB cache vars for upstream (some versions read these)
set(USB_FOUND         TRUE                    CACHE BOOL     "libusb found"       FORCE)
set(USB_INCLUDE_DIR   "${LIBUSB_INCLUDE_DIR}" CACHE PATH     "libusb include dir" FORCE)
set(USB_INCLUDE_DIRS  "${LIBUSB_INCLUDE_DIR}" CACHE PATH     "libusb include dir" FORCE)
set(USB_LIBRARY       "${LIBUSB_LIBRARY}"     CACHE FILEPATH "libusb library"     FORCE)
set(USB_LIBRARIES     "${LIBUSB_LIBRARY}"     CACHE FILEPATH "libusb libraries"   FORCE)
set(USB_LIB           "${LIBUSB_LIBRARY}"     CACHE FILEPATH "libusb library"     FORCE)

# ---------- Fetch whoenig/crazyflie_cpp ----------
FetchContent_Declare(
  crazyflie_cpp
  GIT_REPOSITORY https://github.com/whoenig/crazyflie_cpp.git
  GIT_TAG        master
  CMAKE_CACHE_ARGS
    -DUSB_FOUND:BOOL=TRUE
    -DUSB_INCLUDE_DIR:PATH=${LIBUSB_INCLUDE_DIR}
    -DUSB_INCLUDE_DIRS:PATH=${LIBUSB_INCLUDE_DIR}
    -DUSB_LIBRARY:FILEPATH=${LIBUSB_LIBRARY}
    -DUSB_LIBRARIES:FILEPATH=${LIBUSB_LIBRARY}
    -DUSB_LIB:FILEPATH=${LIBUSB_LIBRARY}
)
FetchContent_MakeAvailable(crazyflie_cpp)

# Strip GCC -W* flags when using MSVC (avoid invalid options)
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
  _strip_gcc_warnings(crazyradio) # if upstream defines it
endif()

# ---------- Executable ----------
add_executable(cf4pwm_runner_win
  src/main_win.cpp
  src/radio_cfclient.cpp
  src/metrics.cpp
  src/pack.cpp
  src/timing_win.cpp
  src/udp_input.cpp
  src/xyz_stream.cpp
)

# Include paths (put UPSTREAM first to avoid picking local copies)
set(_cf_src_dir "${crazyflie_cpp_SOURCE_DIR}")
target_include_directories(cf4pwm_runner_win PRIVATE
  "${_cf_src_dir}/include"
  "${_cf_src_dir}/include/crazyflie_cpp"
  "${LIBUSB_INCLUDE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# Ensure crazyflie_cpp sees libusb includes/libs, and enforce MSVC packing
if (TARGET crazyflie_cpp)
  target_include_directories(crazyflie_cpp PRIVATE "${LIBUSB_INCLUDE_DIR}")
  target_link_libraries(crazyflie_cpp     PRIVATE "${LIBUSB_LIBRARY}")
  if (MSVC)
    target_compile_options(crazyflie_cpp PRIVATE "/I${LIBUSB_INCLUDE_DIR}")
    target_compile_options(crazyflie_cpp PRIVATE /Zp1 /utf-8)   # 1-byte struct packing + UTF-8
  endif()
endif()

# Link upstream + libusb + Winsock + WinMM
if (WIN32)
  target_link_libraries(cf4pwm_runner_win PRIVATE crazyflie_cpp "${LIBUSB_LIBRARY}" Ws2_32 Winmm)
else()
  target_link_libraries(cf4pwm_runner_win PRIVATE crazyflie_cpp "${LIBUSB_LIBRARY}")
endif()

# Output dir
set_target_properties(cf4pwm_runner_win PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ---------- MSVC: force-include POSIX attr shim on both targets ----------
if (MSVC)
  get_filename_component(_shim_abs
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cf4pwm/msvc_posix_attr_shim.hpp" ABSOLUTE)
  target_compile_options(cf4pwm_runner_win PRIVATE /FI"${_shim_abs}")
  if (TARGET crazyflie_cpp)
    target_compile_options(crazyflie_cpp PRIVATE /FI"${_shim_abs}")
  endif()

  # Your executable TU also needs pack=1 and UTF-8
  target_compile_options(cf4pwm_runner_win PRIVATE /Zp1 /utf-8)
else()
  target_compile_options(cf4pwm_runner_win PRIVATE -Wall -Wextra)
endif()

# (Optional) extra safety: enforce /Zp1 on specific TU if needed
if (MSVC)
  set_source_files_properties(
    src/main_win.cpp src/radio_cfclient.cpp
    PROPERTIES COMPILE_OPTIONS "/Zp1;/utf-8"
  )
endif()
