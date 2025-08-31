# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-src")
  file(MAKE_DIRECTORY "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-src")
endif()
file(MAKE_DIRECTORY
  "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-build"
  "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-subbuild/crazyflie_cpp-populate-prefix"
  "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-subbuild/crazyflie_cpp-populate-prefix/tmp"
  "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-subbuild/crazyflie_cpp-populate-prefix/src/crazyflie_cpp-populate-stamp"
  "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-subbuild/crazyflie_cpp-populate-prefix/src"
  "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-subbuild/crazyflie_cpp-populate-prefix/src/crazyflie_cpp-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-subbuild/crazyflie_cpp-populate-prefix/src/crazyflie_cpp-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-subbuild/crazyflie_cpp-populate-prefix/src/crazyflie_cpp-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
