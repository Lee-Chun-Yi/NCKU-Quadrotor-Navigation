cmake_minimum_required(VERSION 3.21)
project(cf4pwm_runner_win LANGUAGES CXX)

option(CFCPP_USE_FETCHCONTENT "Use FetchContent to get whoenig/crazyflie_cpp" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CFCPP_USE_FETCHCONTENT)
  include(FetchContent)
  FetchContent_Declare(crazyflie_cpp
    GIT_REPOSITORY https://github.com/whoenig/crazyflie_cpp.git
    GIT_TAG master
  )
  FetchContent_MakeAvailable(crazyflie_cpp)
else()
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../crazyflie_cpp crazyflie_cpp)
  set(crazyflie_cpp_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../crazyflie_cpp)
endif()

add_executable(cf4pwm_runner_win
  src/main_win.cpp
  src/udp_input.cpp
  src/timing_win.cpp
  src/metrics.cpp
  src/radio_cfclient.cpp
  src/pack.cpp
)

target_include_directories(cf4pwm_runner_win
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${crazyflie_cpp_SOURCE_DIR}/include
)

if(MSVC)
  get_target_property(_copts crazyflie_cpp COMPILE_OPTIONS)
  if(_copts)
    list(FILTER _copts EXCLUDE REGEX "^-W(extra|all|error)$")
    set_property(TARGET crazyflie_cpp PROPERTY COMPILE_OPTIONS "${_copts}")
  endif()
  target_compile_options(crazyflie_cpp PRIVATE /W3)

  target_compile_options(cf4pwm_runner_win PRIVATE /W3)
  target_compile_definitions(cf4pwm_runner_win PRIVATE NOMINMAX)
endif()

target_link_libraries(cf4pwm_runner_win PRIVATE crazyflie_cpp Ws2_32)

