# - Find the MySQL installation.
# In Windows, we make the assumption that, if the MySQL files are installed, the default directory
# will be C:\Program Files\MySQL.
#
# This module defines
#  MySQL_LIBRARIES - the MySQL libraries needed for linking
#  MySQL_INCLUDE_DIRS - the directories of the MySQL headers
#  MySQL_VERSION_STRING - the version of MySQL found (since CMake 2.8.8)

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
# MySQL_ADDITIONAL_VERSIONS is a variable that can be used to set the
# version mumber of the implementation of MySQL.
# In Windows the default installation of MySQL uses that as part of the path.
# E.g C:\Program Files\MySQL\8.4.
# Currently, the following version numbers are known to this module:
# "5.5" "5.4" "5.3" "5.2" "5.1" "5.0"
#
# To use this variable just do something like this:
# set(MySQL_ADDITIONAL_VERSIONS "5.6" "5.5.2")
# before calling find_package(MySQL) in your CMakeLists.txt file.
# This will mean that the versions you set here will be found first in the order
# specified before the default ones are searched.
#
# ----------------------------------------------------------------------------
# You may need to manually set:
#  MySQL_INCLUDE_DIR  - the path to where the MySQL include files are.
#  MySQL_LIBRARY_DIR  - The path to where the MySQL library files are.
# If FindMySQL.cmake cannot find the include files or the library files.
#
# ----------------------------------------------------------------------------
# The following variables are set if MySQL is found:
#  MySQL_FOUND         - Set to true when MySQL is found.
#  MySQL_INCLUDE_DIRS  - Include directories for MySQL
#  MySQL_LIBRARY_DIRS  - Link directories for MySQL libraries
#  MySQL_LIBRARIES     - The MySQL libraries.
#
# ----------------------------------------------------------------------------
# If you have installed MySQL in a non-standard location.
# (Please note that in the following comments, it is assumed that <Your Path>
# points to the root directory of the include directory of MySQL.)
# Then you have three options.
# 1) After CMake runs, set MySQL_INCLUDE_DIR to <Your Path>/include and
#    MySQL_LIBRARY_DIR to wherever the library pq (or libpq in windows) is
# 2) Use CMAKE_INCLUDE_PATH to set a path to <Your Path>/MySQL<-version>. This will allow find_path()
#    to locate MySQL_INCLUDE_DIR by utilizing the PATH_SUFFIXES option. e.g. In your CMakeLists.txt file
#    set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "<Your Path>/include")
# 3) Set an environment variable called ${MySQL_ROOT} that points to the root of where you have
#    installed MySQL, e.g. <Your Path>.
#
# ----------------------------------------------------------------------------

set(MySQL_INCLUDE_PATH_DESCRIPTION "top-level directory containing the MySQL include directories. E.g /usr/local/include/MySQL/5.3 or C:/Program Files/MySQL/5.3/include")
set(MySQL_INCLUDE_DIR_MESSAGE "Set the MySQL_INCLUDE_DIR cmake cache entry to the ${MySQL_INCLUDE_PATH_DESCRIPTION}")
set(MySQL_LIBRARY_PATH_DESCRIPTION "top-level directory containing the MySQL libraries.")
set(MySQL_LIBRARY_DIR_MESSAGE "Set the MySQL_LIBRARY_DIR cmake cache entry to the ${MySQL_LIBRARY_PATH_DESCRIPTION}")
set(MySQL_ROOT_DIR_MESSAGE "Set the MySQL_ROOT system variable to where MySQL is found on the machine E.g C:/Program Files/MySQL/5.3")


set(MySQL_KNOWN_VERSIONS ${MySQL_ADDITIONAL_VERSIONS}
    "5.5" "5.4" "5.3" "5.2" "5.1" "5.0")

# Define additional search paths for root directories.
if ( WIN32 )
  foreach (suffix ${MySQL_KNOWN_VERSIONS} )
    set(MySQL_ADDITIONAL_SEARCH_PATHS ${MySQL_ADDITIONAL_SEARCH_PATHS} "C:/Program Files/MySQL/${suffix}" )
  endforeach()
endif()
set( MySQL_ROOT_DIRECTORIES
   ENV MySQL_ROOT
   ${MySQL_ROOT}
   ${MySQL_ADDITIONAL_SEARCH_PATHS}
)

#
# Look for an installation.
#
find_path(MySQL_INCLUDE_DIR
  NAMES mysql.h
  PATHS
   # Look in other places.
   ${MySQL_ROOT_DIRECTORIES}
  PATH_SUFFIXES
    mysql
    include
  # Help the user find it if we cannot.
  DOC "The ${MySQL_INCLUDE_DIR_MESSAGE}"
)

# The MySQL library.
set (MySQL_LIBRARY_TO_FIND mysqlclient)
# Setting some more prefixes for the library
set (MySQL_LIB_PREFIX "")
if ( WIN32 )
  set (MySQL_LIB_PREFIX ${MySQL_LIB_PREFIX} "lib")
  set ( MySQL_LIBRARY_TO_FIND ${MySQL_LIB_PREFIX}${MySQL_LIBRARY_TO_FIND})
endif()

find_library( MySQL_LIBRARY
 NAMES ${MySQL_LIBRARY_TO_FIND}
 PATHS
   ${MySQL_ROOT_DIRECTORIES}
 PATH_SUFFIXES
   lib
)
get_filename_component(MySQL_LIBRARY_DIR ${MySQL_LIBRARY} PATH)

if (MySQL_INCLUDE_DIR AND EXISTS "${MySQL_INCLUDE_DIR}/mysql_version.h")
  file(STRINGS "${MySQL_INCLUDE_DIR}/mysql_version.h" mysql_version_str
       REGEX "^#define[\t ]+MYSQL_SERVER_VERSION[\t ]+\".*\"")

  string(REGEX REPLACE "^#define[\t ]+MYSQL_SERVER_VERSION[\t ]+\"([^\"]*)\".*" "\\1"
         MySQL_VERSION_STRING "${mysql_version_str}")
  unset(mysql_version_str)
endif()

# Did we find anything?
#include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MySQL
                                  REQUIRED_VARS MySQL_LIBRARY MySQL_INCLUDE_DIR
                                  VERSION_VAR MySQL_VERSION_STRING)
set( MySQL_FOUND  ${MYSQL_FOUND})

# Now try to get the include and library path.
if(MySQL_FOUND)

  set(MySQL_INCLUDE_DIRS ${MySQL_INCLUDE_DIR} ${MySQL_TYPE_INCLUDE_DIR} )
  set(MySQL_LIBRARY_DIRS ${MySQL_LIBRARY_DIR} )
  set(MySQL_LIBRARIES ${MySQL_LIBRARY_TO_FIND})

  #message("Final MySQL include dir: ${MySQL_INCLUDE_DIRS}")
  #message("Final MySQL library dir: ${MySQL_LIBRARY_DIRS}")
  #message("Final MySQL libraries:   ${MySQL_LIBRARIES}")
endif()

mark_as_advanced(MySQL_INCLUDE_DIR MySQL_TYPE_INCLUDE_DIR MySQL_LIBRARY )
