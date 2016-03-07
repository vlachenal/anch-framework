# - Find the SQLite3 installation.
# In Windows, we make the assumption that, if the SQLite3 files are installed, the default directory
# will be C:\Program Files\SQLite3.
#
# This module defines
#  SQLite3_LIBRARIES - the SQLite3 libraries needed for linking
#  SQLite3_INCLUDE_DIRS - the directories of the SQLite3 headers
#  SQLite3_VERSION_STRING - the version of SQLite3 found (since CMake 2.8.8)

#=============================================================================
# Copyright 2004-2009 Kitware, Inc.
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

# ----------------------------------------------------------------------------
# History:
# This module is derived from the module originally found in the VTK source tree.
#
# ----------------------------------------------------------------------------
# Note:
# SQLite3_ADDITIONAL_VERSIONS is a variable that can be used to set the
# version mumber of the implementation of SQLite3.
# In Windows the default installation of SQLite3 uses that as part of the path.
# E.g C:\Program Files\SQLite3\8.4.
# Currently, the following version numbers are known to this module:
# "5.5" "5.4" "5.3" "5.2" "5.1" "5.0"
#
# To use this variable just do something like this:
# set(SQLite3_ADDITIONAL_VERSIONS "5.6" "5.5.2")
# before calling find_package(SQLite3) in your CMakeLists.txt file.
# This will mean that the versions you set here will be found first in the order
# specified before the default ones are searched.
#
# ----------------------------------------------------------------------------
# You may need to manually set:
#  SQLite3_INCLUDE_DIR  - the path to where the SQLite3 include files are.
#  SQLite3_LIBRARY_DIR  - The path to where the SQLite3 library files are.
# If FindSQLite3.cmake cannot find the include files or the library files.
#
# ----------------------------------------------------------------------------
# The following variables are set if SQLite3 is found:
#  SQLite3_FOUND         - Set to true when SQLite3 is found.
#  SQLite3_INCLUDE_DIRS  - Include directories for SQLite3
#  SQLite3_LIBRARY_DIRS  - Link directories for SQLite3 libraries
#  SQLite3_LIBRARIES     - The SQLite3 libraries.
#
# ----------------------------------------------------------------------------
# If you have installed SQLite3 in a non-standard location.
# (Please note that in the following comments, it is assumed that <Your Path>
# points to the root directory of the include directory of SQLite3.)
# Then you have three options.
# 1) After CMake runs, set SQLite3_INCLUDE_DIR to <Your Path>/include and
#    SQLite3_LIBRARY_DIR to wherever the library pq (or libpq in windows) is
# 2) Use CMAKE_INCLUDE_PATH to set a path to <Your Path>/SQLite3<-version>. This will allow find_path()
#    to locate SQLite3_INCLUDE_DIR by utilizing the PATH_SUFFIXES option. e.g. In your CMakeLists.txt file
#    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "<Your Path>/include")
# 3) Set an environment variable called ${SQLite3_ROOT} that points to the root of where you have
#    installed SQLite3, e.g. <Your Path>.
#
# ----------------------------------------------------------------------------

set(SQLite3_INCLUDE_PATH_DESCRIPTION "top-level directory containing the SQLite3 include directories. E.g /usr/local/include/SQLite3/5.3 or C:/Program Files/SQLite3/5.3/include")
set(SQLite3_INCLUDE_DIR_MESSAGE "Set the SQLite3_INCLUDE_DIR cmake cache entry to the ${SQLite3_INCLUDE_PATH_DESCRIPTION}")
set(SQLite3_LIBRARY_PATH_DESCRIPTION "top-level directory containing the SQLite3 libraries.")
set(SQLite3_LIBRARY_DIR_MESSAGE "Set the SQLite3_LIBRARY_DIR cmake cache entry to the ${SQLite3_LIBRARY_PATH_DESCRIPTION}")
set(SQLite3_ROOT_DIR_MESSAGE "Set the SQLite3_ROOT system variable to where SQLite3 is found on the machine E.g C:/Program Files/SQLite3/5.3")


set(SQLite3_KNOWN_VERSIONS ${SQLite3_ADDITIONAL_VERSIONS}
    "5.5" "5.4" "5.3" "5.2" "5.1" "5.0")

# Define additional search paths for root directories.
if ( WIN32 )
  foreach (suffix ${SQLite3_KNOWN_VERSIONS} )
    set(SQLite3_ADDITIONAL_SEARCH_PATHS ${SQLite3_ADDITIONAL_SEARCH_PATHS} "C:/Program Files/SQLite3/${suffix}" )
  endforeach()
endif()
set( SQLite3_ROOT_DIRECTORIES
   ENV SQLite3_ROOT
   ${SQLite3_ROOT}
   ${SQLite3_ADDITIONAL_SEARCH_PATHS}
)

#
# Look for an installation.
#
find_path(SQLite3_INCLUDE_DIR
  NAMES sqlite3.h
  PATHS
   # Look in other places.
   ${SQLite3_ROOT_DIRECTORIES}
  PATH_SUFFIXES
    sqlite3
    include
  # Help the user find it if we cannot.
  DOC "The ${SQLite3_INCLUDE_DIR_MESSAGE}"
)

# The SQLite3 library.
set (SQLite3_LIBRARY_TO_FIND sqlite3)
# Setting some more prefixes for the library
set (SQLite3_LIB_PREFIX "")
if ( WIN32 )
  set (SQLite3_LIB_PREFIX ${SQLite3_LIB_PREFIX} "lib")
  set ( SQLite3_LIBRARY_TO_FIND ${SQLite3_LIB_PREFIX}${SQLite3_LIBRARY_TO_FIND})
endif()

find_library( SQLite3_LIBRARY
 NAMES ${SQLite3_LIBRARY_TO_FIND}
 PATHS
   ${SQLite3_ROOT_DIRECTORIES}
 PATH_SUFFIXES
   lib
)
get_filename_component(SQLite3_LIBRARY_DIR ${SQLite3_LIBRARY} PATH)

if (SQLite3_INCLUDE_DIR AND EXISTS "${SQLite3_INCLUDE_DIR}/sqlite3.h")
  file(STRINGS "${SQLite3_INCLUDE_DIR}/sqlite3.h" sqlite3_version_str
       REGEX "^#define[\t ]+SQLITE_VERSION[\t ]+\".*\"")

  string(REGEX REPLACE "^#define[\t ]+SQLITE_VERSION[\t ]+\"([^\"]*)\".*" "\\1"
         SQLite3_VERSION_STRING "${sqlite3_version_str}")
  unset(sqlite3_version_str)
endif()

# Did we find anything?
#include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3
                                  REQUIRED_VARS SQLite3_LIBRARY SQLite3_INCLUDE_DIR
                                  VERSION_VAR SQLite3_VERSION_STRING)
set( SQLite3_FOUND  ${SQLITE3_FOUND})

# Now try to get the include and library path.
if(SQLite3_FOUND)

  set(SQLite3_INCLUDE_DIRS ${SQLite3_INCLUDE_DIR} ${SQLite3_TYPE_INCLUDE_DIR} )
  set(SQLite3_LIBRARY_DIRS ${SQLite3_LIBRARY_DIR} )
  set(SQLite3_LIBRARIES ${SQLite3_LIBRARY_TO_FIND})

  #message("Final SQLite3 include dir: ${SQLite3_INCLUDE_DIRS}")
  #message("Final SQLite3 library dir: ${SQLite3_LIBRARY_DIRS}")
  #message("Final SQLite3 libraries:   ${SQLite3_LIBRARIES}")
endif()

mark_as_advanced(SQLite3_INCLUDE_DIR SQLite3_TYPE_INCLUDE_DIR SQLite3_LIBRARY )
