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

# ---------- vcpkg: 找 libusb ----------
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

# 也把上游常見變數一併預先設好（有些版本會用到）
set(USB_FOUND         TRUE                          CACHE BOOL     "libusb found"       FORCE)
set(USB_INCLUDE_DIR   "${LIBUSB_INCLUDE_DIR}"       CACHE PATH     "libusb include dir" FORCE)
set(USB_INCLUDE_DIRS  "${LIBUSB_INCLUDE_DIR}"       CACHE PATH     "libusb include dir" FORCE)
set(USB_LIBRARY       "${LIBUSB_LIBRARY}"           CACHE FILEPATH "libusb library"     FORCE)
set(USB_LIBRARIES     "${LIBUSB_LIBRARY}"           CACHE FILEPATH "libusb libraries"   FORCE)
set(USB_LIB           "${LIBUSB_LIBRARY}"           CACHE FILEPATH "libusb library"     FORCE)

# ---------- 抓取 crazyflie_cpp ----------
# 用 CMAKE_CACHE_ARGS 確保子組態能收到 USB_* 參數
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

# MSVC：移除上游遺留的 GCC 警告旗標，避免 MSVC 誤判
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
  _strip_gcc_warnings(crazyradio)
endif()

# ---------- 我們的執行檔 ----------
add_executable(cf4pwm_runner_win
  src/main_win.cpp
  src/radio_cfclient.cpp
  src/metrics.cpp
  src/pack.cpp
  src/timing_win.cpp
  src/udp_input.cpp
)

# include 路徑
set(_cf_src_dir "${crazyflie_cpp_SOURCE_DIR}")
target_include_directories(cf4pwm_runner_win PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/include"
  "${_cf_src_dir}/include"
  "${_cf_src_dir}/include/crazyflie_cpp"
  "${LIBUSB_INCLUDE_DIR}"
)

# 連結上游 + libusb + Winsock（udp_input 需要）
if (WIN32)
  target_link_libraries(cf4pwm_runner_win PRIVATE crazyflie_cpp "${LIBUSB_LIBRARY}" Ws2_32)
else()
  target_link_libraries(cf4pwm_runner_win PRIVATE crazyflie_cpp "${LIBUSB_LIBRARY}")
endif()

# 輸出目錄
set_target_properties(cf4pwm_runner_win PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# ---------- MSVC 特殊處理 ----------
if (MSVC)
  # 強制包含 shim（純相容：不動 pack）
  get_filename_component(_shim_abs
    "${CMAKE_CURRENT_SOURCE_DIR}/include/cf4pwm/msvc_posix_attr_shim.hpp" ABSOLUTE)
  target_compile_options(cf4pwm_runner_win PRIVATE /FI"${_shim_abs}")
  if (TARGET crazyflie_cpp)
    target_compile_options(crazyflie_cpp PRIVATE /FI"${_shim_abs}")
  endif()

  # 關鍵：讓 crazyflie_cpp 以 1-byte 對齊，避免 CRTP 封包大小 assert
  if (TARGET crazyflie_cpp)
    target_compile_options(crazyflie_cpp PRIVATE /Zp1)
  endif()

  target_compile_options(cf4pwm_runner_win PRIVATE /W3)
else()
  target_compile_options(cf4pwm_runner_win PRIVATE -Wall -Wextra)
endif()
