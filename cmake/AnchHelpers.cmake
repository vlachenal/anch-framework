# AnCH Framework CMake helpers
#
# Functions shared by every AnCH module to declare libraries and test executables
# with the same compilation options, include directories and installation rules.

include_guard(GLOBAL)

include(GNUInstallDirs)
include(GenerateExportHeader)

# Build options +
# Apply AnCH warnings and hardening options to a (non INTERFACE) target.
# These options are PRIVATE: they are never propagated to library consumers.
function(anch_target_build_options target)
  target_compile_options(${target} PRIVATE
    -Wall -Wextra -Wuninitialized -Wconversion -Wsign-conversion -Wformat-security)
  if(ANCH_WARNINGS_AS_ERRORS)
    target_compile_options(${target} PRIVATE -Werror)
  endif()
  # Explicit Release optimization level: placed after CMAKE_CXX_FLAGS_RELEASE on the command line, it overrides it
  target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:-O${ANCH_OPTIMIZATION_LEVEL}>)
  if(ANCH_NATIVE_ARCH)
    target_compile_options(${target} PRIVATE -march=native)
  endif()
  if(ANCH_HARDENING)
    target_compile_options(${target} PRIVATE
      -fstack-protector-all -Wstack-protector --param=ssp-buffer-size=4 -ftrapv)
    # _FORTIFY_SOURCE requires optimization: defining it in Debug makes glibc emit a #warning (error with -Werror)
    target_compile_definitions(${target} PRIVATE
      $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:_FORTIFY_SOURCE=2>)
    # Linker hardening has to be passed to the linker, not to the compiler
    target_link_options(${target} PRIVATE LINKER:-z,relro,-z,now)
  endif()
endfunction()
# Build options -

# Libraries +
# Declare an AnCH library.
#
# anch_add_library(<target>
#   [HEADER_ONLY]
#   [OUTPUT_NAME <name>]              library file name (default: <target>)
#   [EXPORT_NAME <name>]              name in the anch:: namespace (default: <target> without 'anch-' prefix)
#   [INCLUDE_DIRS <dir>...]           public include directories (default: inc)
#   [SOURCES <file>...]               sources (default: every src/*.cpp)
#   [PUBLIC_DEPS <target>...]         dependencies exposed by public headers
#   [PRIVATE_DEPS <target>...]        implementation-only dependencies
#   [PUBLIC_DEFINITIONS <def>...]     definitions used by public headers
#   [PRIVATE_DEFINITIONS <def>...])
#
# Creates <target> with an anch::<export name> alias, and registers its installation
# (library, headers in ${CMAKE_INSTALL_INCLUDEDIR}/anch, and AnchTargets export set).
#
# Compiled libraries are built with hidden symbols visibility: only classes, functions and
# variables marked with the <TARGET>_EXPORT macro (ANCH_CONF_EXPORT for anch-conf, ...)
# are exported. The macro is defined in the generated "<target>-export.hpp" header.
function(anch_add_library target)
  cmake_parse_arguments(PARSE_ARGV 1 ARG
    "HEADER_ONLY"
    "OUTPUT_NAME;EXPORT_NAME"
    "INCLUDE_DIRS;SOURCES;PUBLIC_DEPS;PRIVATE_DEPS;PUBLIC_DEFINITIONS;PRIVATE_DEFINITIONS")

  if(NOT ARG_INCLUDE_DIRS)
    set(ARG_INCLUDE_DIRS inc)
  endif()
  if(NOT ARG_EXPORT_NAME)
    string(REGEX REPLACE "^anch-" "" ARG_EXPORT_NAME ${target})
  endif()

  set(build_includes)
  foreach(dir IN LISTS ARG_INCLUDE_DIRS)
    list(APPEND build_includes $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/${dir}>)
  endforeach()

  if(ARG_HEADER_ONLY)
    add_library(${target} INTERFACE)
    set(scope INTERFACE)
  else()
    if(NOT ARG_SOURCES)
      file(GLOB_RECURSE ARG_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
    endif()
    add_library(${target} ${ARG_SOURCES})
    set(scope PUBLIC)
    anch_target_build_options(${target})
    if(ARG_OUTPUT_NAME)
      set_target_properties(${target} PROPERTIES OUTPUT_NAME ${ARG_OUTPUT_NAME})
    endif()
    set_target_properties(${target} PROPERTIES
      VERSION ${PROJECT_VERSION}
      SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})
    target_link_libraries(${target} PRIVATE ${ARG_PRIVATE_DEPS})
    target_compile_definitions(${target} PRIVATE ${ARG_PRIVATE_DEFINITIONS})

    # Exported symbols +
    # Inline functions keep the visibility of their class: with 'extern template' declarations, consumers built
    # without optimization call the inline members of explicit instantiations exported by the library
    set_target_properties(${target} PROPERTIES CXX_VISIBILITY_PRESET hidden)
    string(MAKE_C_IDENTIFIER ${target} export_base)
    string(TOUPPER ${export_base} export_base)
    set(export_dir ${CMAKE_CURRENT_BINARY_DIR}/export)
    set(export_header ${export_dir}/${target}-export.hpp)
    generate_export_header(${target}
      BASE_NAME ${export_base}
      EXPORT_FILE_NAME ${export_header})
    list(APPEND build_includes $<BUILD_INTERFACE:${export_dir}>)
    if(NOT BUILD_SHARED_LIBS)
      target_compile_definitions(${target} PUBLIC ${export_base}_STATIC_DEFINE)
    endif()
    install(FILES ${export_header} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/anch)
    # Exported symbols -
  endif()

  add_library(anch::${ARG_EXPORT_NAME} ALIAS ${target})
  set_target_properties(${target} PROPERTIES EXPORT_NAME ${ARG_EXPORT_NAME})

  target_compile_features(${target} ${scope} cxx_std_23)
  target_include_directories(${target} ${scope}
    ${build_includes}
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/anch>)
  target_link_libraries(${target} ${scope} ${ARG_PUBLIC_DEPS})
  target_compile_definitions(${target} ${scope} ${ANCH_PLATFORM_DEFINITION} ${ARG_PUBLIC_DEFINITIONS})

  # Installation +
  if(ANCH_INSTALL_RPATH AND NOT APPLE)
    # Dependencies (and SQL drivers loaded with dlopen) are installed in the same directory
    set_target_properties(${target} PROPERTIES INSTALL_RPATH "$ORIGIN")
  endif()
  install(TARGETS ${target}
    EXPORT AnchTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
  foreach(dir IN LISTS ARG_INCLUDE_DIRS)
    install(DIRECTORY ${dir}/
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/anch
      FILES_MATCHING PATTERN "*.hpp")
  endforeach()
  # Installation -
endfunction()
# Libraries -

# Tests +
# Declare an AnCH test (or utility) executable.
#
# anch_add_executable(<target>
#   SOURCES <file>...
#   [LIBS <target>...])
function(anch_add_executable target)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "SOURCES;LIBS")
  add_executable(${target} ${ARG_SOURCES})
  anch_target_build_options(${target})
  target_link_libraries(${target} PRIVATE ${ARG_LIBS})
endfunction()

# Declare a documentation example (<module>/examples/<file>.cpp, included in documentation with \snippet).
# Examples are built with AnCH options. A RUN example is also a test (labels: module label and 'examples'), run
# from the current binary directory: it has to work without database nor network service.
#
# anch_add_example(<name>
#   SOURCES <file>...
#   [LIBS <target>...]
#   [RUN [ARGS <argument>...]])
function(anch_add_example name)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "RUN" "" "SOURCES;LIBS;ARGS")
  set(target anch-example-${name})
  anch_add_executable(${target} SOURCES ${ARG_SOURCES} LIBS ${ARG_LIBS})
  if(ARG_RUN)
    add_test(NAME example-${name} COMMAND ${target} ${ARG_ARGS})
    get_property(labels DIRECTORY PROPERTY LABELS)
    set_tests_properties(example-${name} PROPERTIES LABELS "${labels};examples")
  endif()
endfunction()

# Install a tool, with a relative RPATH to installed libraries (ANCH_INSTALL_RPATH).
#
# anch_install_executable(<target>)
function(anch_install_executable target)
  if(ANCH_INSTALL_RPATH AND NOT APPLE)
    file(RELATIVE_PATH libdir ${CMAKE_INSTALL_FULL_BINDIR} ${CMAKE_INSTALL_FULL_LIBDIR})
    set_target_properties(${target} PROPERTIES INSTALL_RPATH "$ORIGIN/${libdir}")
  endif()
  install(TARGETS ${target} RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endfunction()

# Copy test fixtures in current binary directory (tests are run from there).
#
# anch_copy_test_files(<file>...)
function(anch_copy_test_files)
  foreach(file IN LISTS ARGN)
    get_filename_component(name ${file} NAME)
    configure_file(${file} ${CMAKE_CURRENT_BINARY_DIR}/${name} COPYONLY)
  endforeach()
endfunction()
# Tests -
