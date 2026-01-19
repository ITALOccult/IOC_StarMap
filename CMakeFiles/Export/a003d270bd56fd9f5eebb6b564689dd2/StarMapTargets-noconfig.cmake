#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "StarMap::starmap" for configuration ""
set_property(TARGET StarMap::starmap APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(StarMap::starmap PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libstarmap.1.0.0.dylib"
  IMPORTED_SONAME_NOCONFIG "@rpath/libstarmap.1.dylib"
  )

list(APPEND _cmake_import_check_targets StarMap::starmap )
list(APPEND _cmake_import_check_files_for_StarMap::starmap "${_IMPORT_PREFIX}/lib/libstarmap.1.0.0.dylib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
