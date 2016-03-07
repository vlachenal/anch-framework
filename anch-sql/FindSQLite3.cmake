# - Find the SQLite installation.
# In Windows, we make the assumption that, if the SQLite files are installed, the default directory
# will be C:\Program Files\SQLite.
#
# This module defines
#  SQLite_LIBRARIES - the SQLite libraries needed for linking
#  SQLite_INCLUDE_DIRS - the directories of the SQLite headers
#  SQLite_VERSION_STRING - the version of SQLite found (since CMake 2.8.8)

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
# SQLite_ADDITIONAL_VERSIONS is a variable that can be used to set the
# version mumber of the implementation of SQLite.
# In Windows the default installation of SQLite uses that as part of the path.
# E.g C:\Program Files\SQLite\8.4.
# Currently, the following version numbers are known to this module:
# "5.5" "5.4" "5.3" "5.2" "5.1" "5.0"
#
# To use this variable just do something like this:
# set(SQLite_ADDITIONAL_VERSIONS "5.6" "5.5.2")
# before calling find_package(SQLite) in your CMakeLists.txt file.
# This will mean that the versions you set here will be found first in the order
# specified before the default ones are searched.
#
# ----------------------------------------------------------------------------
# You may need to manually set:
#  SQLite_INCLUDE_DIR  - the path to where the SQLite include files are.
#  SQLite_LIBRARY_DIR  - The path to where the SQLite library files are.
# If FindSQLite.cmake cannot find the include files or the library files.
#
# ----------------------------------------------------------------------------
# The following variables are set if SQLite is found:
#  SQLite_FOUND         - Set to true when SQLite is found.
#  SQLite_INCLUDE_DIRS  - Include directories for SQLite
#  SQLite_LIBRARY_DIRS  - Link directories for SQLite libraries
#  SQLite_LIBRARIES     - The SQLite libraries.
#
# ----------------------------------------------------------------------------
# If you have installed SQLite in a non-standard location.
# (Please note that in the following comments, it is assumed that <Your Path>
# points to the root directory of the include directory of SQLite.)
# Then you have three options.
# 1) After CMake runs, set SQLite_INCLUDE_DIR to <Your Path>/include and
#    SQLite_LIBRARY_DIR to wherever the library pq (or libpq in windows) is
# 2) Use CMAKE_INCLUDE_PATH to set a path to <Your Path>/SQLite<-version>. This will allow find_path()
#    to locate SQLite_INCLUDE_DIR by utilizing the PATH_SUFFIXES option. e.g. In your CMakeLists.txt file
#    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "<Your Path>/include")
# 3) Set an environment variable called ${SQLite_ROOT} that points to the root of where you have
#    installed SQLite, e.g. <Your Path>.
#
# ----------------------------------------------------------------------------

set(SQLite_INCLUDE_PATH_DESCRIPTION "top-level directory containing the SQLite include directories. E.g /usr/local/include/SQLite/5.3 or C:/Program Files/SQLite/5.3/include")
set(SQLite_INCLUDE_DIR_MESSAGE "Set the SQLite_INCLUDE_DIR cmake cache entry to the ${SQLite_INCLUDE_PATH_DESCRIPTION}")
set(SQLite_LIBRARY_PATH_DESCRIPTION "top-level directory containing the SQLite libraries.")
set(SQLite_LIBRARY_DIR_MESSAGE "Set the SQLite_LIBRARY_DIR cmake cache entry to the ${SQLite_LIBRARY_PATH_DESCRIPTION}")
set(SQLite_ROOT_DIR_MESSAGE "Set the SQLite_ROOT system variable to where SQLite is found on the machine E.g C:/Program Files/SQLite/5.3")


set(SQLite_KNOWN_VERSIONS ${SQLite_ADDITIONAL_VERSIONS}
    "5.5" "5.4" "5.3" "5.2" "5.1" "5.0")

# Define additional search paths for root directories.
if ( WIN32 )
  foreach (suffix ${SQLite_KNOWN_VERSIONS} )
    set(SQLite_ADDITIONAL_SEARCH_PATHS ${SQLite_ADDITIONAL_SEARCH_PATHS} "C:/Program Files/SQLite/${suffix}" )
  endforeach()
endif()
set( SQLite_ROOT_DIRECTORIES
   ENV SQLite_ROOT
   ${SQLite_ROOT}
   ${SQLite_ADDITIONAL_SEARCH_PATHS}
)

#
# Look for an installation.
#
find_path(SQLite_INCLUDE_DIR
  NAMES sqlite.h
  PATHS
   # Look in other places.
   ${SQLite_ROOT_DIRECTORIES}
  PATH_SUFFIXES
    sqlite
    include
  # Help the user find it if we cannot.
  DOC "The ${SQLite_INCLUDE_DIR_MESSAGE}"
)

# The SQLite library.
set (SQLite_LIBRARY_TO_FIND sqliteclient)
# Setting some more prefixes for the library
set (SQLite_LIB_PREFIX "")
if ( WIN32 )
  set (SQLite_LIB_PREFIX ${SQLite_LIB_PREFIX} "lib")
  set ( SQLite_LIBRARY_TO_FIND ${SQLite_LIB_PREFIX}${SQLite_LIBRARY_TO_FIND})
endif()

find_library( SQLite_LIBRARY
 NAMES ${SQLite_LIBRARY_TO_FIND}
 PATHS
   ${SQLite_ROOT_DIRECTORIES}
 PATH_SUFFIXES
   lib
)
get_filename_component(SQLite_LIBRARY_DIR ${SQLite_LIBRARY} PATH)

if (SQLite_INCLUDE_DIR AND EXISTS "${SQLite_INCLUDE_DIR}/sqlite_version.h")
  file(STRINGS "${SQLite_INCLUDE_DIR}/sqlite_version.h" sqlite_version_str
       REGEX "^#define[\t ]+SQLITE_SERVER_VERSION[\t ]+\".*\"")

  string(REGEX REPLACE "^#define[\t ]+SQLITE_SERVER_VERSION[\t ]+\"([^\"]*)\".*" "\\1"
         SQLite_VERSION_STRING "${sqlite_version_str}")
  unset(sqlite_version_str)
endif()

# Did we find anything?
#include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite
                                  REQUIRED_VARS SQLite_LIBRARY SQLite_INCLUDE_DIR
                                  VERSION_VAR SQLite_VERSION_STRING)
set( SQLite_FOUND  ${SQLITE_FOUND})

# Now try to get the include and library path.
if(SQLite_FOUND)

  set(SQLite_INCLUDE_DIRS ${SQLite_INCLUDE_DIR} ${SQLite_TYPE_INCLUDE_DIR} )
  set(SQLite_LIBRARY_DIRS ${SQLite_LIBRARY_DIR} )
  set(SQLite_LIBRARIES ${SQLite_LIBRARY_TO_FIND})

  #message("Final SQLite include dir: ${SQLite_INCLUDE_DIRS}")
  #message("Final SQLite library dir: ${SQLite_LIBRARY_DIRS}")
  #message("Final SQLite libraries:   ${SQLite_LIBRARIES}")
endif()

mark_as_advanced(SQLite_INCLUDE_DIR SQLite_TYPE_INCLUDE_DIR SQLite_LIBRARY )
