# Install script for directory: /Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/libstarmap.1.0.0.dylib"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/libstarmap.1.dylib"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libstarmap.1.0.0.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libstarmap.1.dylib"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" -x "${file}")
      endif()
    endif()
  endforeach()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/libstarmap.dylib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/starmap" TYPE FILE FILES
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/core/Coordinates.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/core/CelestialObject.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/catalog/GaiaClient.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/catalog/SAOCatalog.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/catalog/CatalogManager.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/catalog/GaiaSAODatabase.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/map/MapConfiguration.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/map/Projection.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/map/MapRenderer.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/map/GridRenderer.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/map/ChartGenerator.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/map/ConstellationData.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/config/ConfigurationLoader.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/config/JSONConfigLoader.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/config/LibraryConfig.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/utils/HttpClient.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/occultation/OccultationData.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/occultation/OccultationChartBuilder.h"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap/StarMap.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE DIRECTORY FILES "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/include/starmap")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/StarMap/StarMapTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/StarMap/StarMapTargets.cmake"
         "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/CMakeFiles/Export/a003d270bd56fd9f5eebb6b564689dd2/StarMapTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/StarMap/StarMapTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/StarMap/StarMapTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/StarMap" TYPE FILE FILES "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/CMakeFiles/Export/a003d270bd56fd9f5eebb6b564689dd2/StarMapTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/StarMap" TYPE FILE FILES "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/CMakeFiles/Export/a003d270bd56fd9f5eebb6b564689dd2/StarMapTargets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/StarMap" TYPE FILE FILES
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/StarMapConfig.cmake"
    "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/StarMapConfigVersion.cmake"
    )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/Users/michelebigi/Documents/Develop/ASTDYN/IOccultCalc/external/IOC_StarMap/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
