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
endif()

add_executable(cf4pwm_runner_win
  src/main_win.cpp
  src/udp_input.cpp
  src/timing_win.cpp
  src/metrics.cpp
  src/radio_cfclient.cpp
  src/pack.cpp
)

target_include_directories(cf4pwm_runner_win PRIVATE include)

target_link_libraries(cf4pwm_runner_win PRIVATE crazyflie_cpp Ws2_32)

