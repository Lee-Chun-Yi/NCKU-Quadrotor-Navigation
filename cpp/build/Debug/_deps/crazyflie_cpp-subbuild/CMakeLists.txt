# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 4.1.1)

# Reject any attempt to use a toolchain file. We must not use one because
# we could be downloading it here. If the CMAKE_TOOLCHAIN_FILE environment
# variable is set, the cache variable will have been initialized from it.
unset(CMAKE_TOOLCHAIN_FILE CACHE)
unset(ENV{CMAKE_TOOLCHAIN_FILE})

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(crazyflie_cpp-populate NONE)


# Pass through things we've already detected in the main project to avoid
# paying the cost of redetecting them again in ExternalProject_Add()
set(GIT_EXECUTABLE [==[C:/Program Files/Git/cmd/git.exe]==])
set(GIT_VERSION_STRING [==[2.50.1.windows.1]==])
set_property(GLOBAL PROPERTY _CMAKE_FindGit_GIT_EXECUTABLE_VERSION
  [==[C:/Program Files/Git/cmd/git.exe;2.50.1.windows.1]==]
)


include(ExternalProject)
ExternalProject_Add(crazyflie_cpp-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/whoenig/crazyflie_cpp.git" "EXTERNALPROJECT_INTERNAL_ARGUMENT_SEPARATOR" "GIT_TAG" "master" "CMAKE_CACHE_ARGS" "-DUSB_FOUND:BOOL=TRUE" "-DUSB_INCLUDE_DIR:PATH=C:/vcpkg/installed/x64-windows/include" "-DUSB_INCLUDE_DIRS:PATH=C:/vcpkg/installed/x64-windows/include" "-DUSB_LIBRARY:FILEPATH=C:/vcpkg/installed/x64-windows/lib/libusb-1.0.lib" "-DUSB_LIBRARIES:FILEPATH=C:/vcpkg/installed/x64-windows/lib/libusb-1.0.lib" "-DUSB_LIB:FILEPATH=C:/vcpkg/installed/x64-windows/lib/libusb-1.0.lib"
                    SOURCE_DIR          "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-src"
                    BINARY_DIR          "C:/Users/USER/Desktop/cpp/build/Debug/_deps/crazyflie_cpp-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
                    USES_TERMINAL_PATCH     YES
)


