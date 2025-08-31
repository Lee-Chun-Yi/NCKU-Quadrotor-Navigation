#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "crazyflie_cpp::crazyflie_cpp" for configuration "Debug"
set_property(TARGET crazyflie_cpp::crazyflie_cpp APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(crazyflie_cpp::crazyflie_cpp PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/lib/crazyflie_cpp.lib"
  )

list(APPEND _cmake_import_check_targets crazyflie_cpp::crazyflie_cpp )
list(APPEND _cmake_import_check_files_for_crazyflie_cpp::crazyflie_cpp "${_IMPORT_PREFIX}/lib/crazyflie_cpp.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
