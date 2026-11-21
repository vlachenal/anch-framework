# AnCH codec: codecs (media types, serialization and deserialization) shared by anch-rest and anch-msg.
# anch-codec has no format dependency ; each format is a plugin library built according to its option, and the
# formats built are listed in the generated codec/all.hpp header (target anch::codec-all).

# Core: concepts, media types, negotiation, limits, Text and formats registry +
anch_add_library(anch-codec
  PUBLIC_DEPS anch::utils)
# Core -

# Plugins +
# Codecs listed in codec/all.hpp (Available), in preference order (Text, which serializes any streamable value, is
# the last one)
set(codec_headers)
set(codec_types)
set(codec_macros)

# JSON (anch-json)
set(ANCH_CODEC_JSON_ENABLED OFF)
if(ANCH_CODEC_JSON)
  set(ANCH_CODEC_JSON_ENABLED ON)
  anch_add_library(anch-codec-json
    INCLUDE_DIRS json/inc
    SOURCES json/src/codec/json.cpp
    PUBLIC_DEPS anch::codec anch::json)
  list(APPEND codec_headers "codec/json.hpp")
  list(APPEND codec_types "anch::codec::Json")
  list(APPEND codec_macros ANCH_CODEC_JSON_ENABLED)
endif()
set(ANCH_CODEC_JSON_ENABLED ${ANCH_CODEC_JSON_ENABLED} PARENT_SCOPE)

# CSV (anch-csv): lists of described objects
set(ANCH_CODEC_CSV_ENABLED OFF)
if(ANCH_CODEC_CSV AND TARGET anch::csv)
  set(ANCH_CODEC_CSV_ENABLED ON)
  anch_add_library(anch-codec-csv
    INCLUDE_DIRS csv/inc
    SOURCES csv/src/codec/csv.cpp
    PUBLIC_DEPS anch::codec anch::csv)
  list(APPEND codec_headers "codec/csv.hpp")
  list(APPEND codec_types "anch::codec::Csv")
  list(APPEND codec_macros ANCH_CODEC_CSV_ENABLED)
elseif(ANCH_CODEC_CSV)
  message(STATUS "anch-csv is not built: anch-codec-csv is not built")
endif()
set(ANCH_CODEC_CSV_ENABLED ${ANCH_CODEC_CSV_ENABLED} PARENT_SCOPE)

# Protocol Buffers (libprotobuf)
set(ANCH_CODEC_PROTOBUF_ENABLED OFF)
if(ANCH_CODEC_PROTOBUF)
  find_package(Protobuf CONFIG QUIET GLOBAL) # GLOBAL: anch-rest tests and examples use its targets
  if(Protobuf_FOUND)
    set(ANCH_CODEC_PROTOBUF_ENABLED ON)
    anch_add_library(anch-codec-protobuf
      INCLUDE_DIRS protobuf/inc
      SOURCES protobuf/src/codec/protobuf.cpp
      PUBLIC_DEPS anch::codec protobuf::libprotobuf)
    # Protobuf changes generated messages layout under AddressSanitizer (PROTOBUF_MESSAGE_GLOBALS): code
    # compiled with ASan crashes with a system libprotobuf built without it
    if(CMAKE_CXX_FLAGS MATCHES "-fsanitize=[^ ]*address")
      target_compile_definitions(anch-codec-protobuf PUBLIC PROTOBUF_MESSAGE_GLOBALS_TEMPORARY_OPTOUT)
    endif()
    list(APPEND codec_headers "codec/protobuf.hpp")
    list(APPEND codec_types "anch::codec::Protobuf" "anch::codec::ProtobufJson")
    list(APPEND codec_macros ANCH_CODEC_PROTOBUF_ENABLED)
  else()
    message(STATUS "Protobuf not found: anch-codec-protobuf is not built")
  endif()
endif()
set(ANCH_CODEC_PROTOBUF_ENABLED ${ANCH_CODEC_PROTOBUF_ENABLED} PARENT_SCOPE)

# Apache Thrift (Thrift and Boost headers)
set(ANCH_CODEC_THRIFT_ENABLED OFF)
if(ANCH_CODEC_THRIFT)
  find_package(Thrift CONFIG QUIET GLOBAL)
  find_package(Boost CONFIG QUIET GLOBAL) # Thrift C++ headers include Boost headers
  if(Thrift_FOUND AND Boost_FOUND)
    set(ANCH_CODEC_THRIFT_ENABLED ON)
    anch_add_library(anch-codec-thrift
      INCLUDE_DIRS thrift/inc
      SOURCES thrift/src/codec/thrift.cpp
      PUBLIC_DEPS anch::codec thrift::thrift Boost::headers)
    list(APPEND codec_headers "codec/thrift.hpp")
    list(APPEND codec_types "anch::codec::ThriftBinary" "anch::codec::ThriftCompact" "anch::codec::ThriftJson")
    list(APPEND codec_macros ANCH_CODEC_THRIFT_ENABLED)
  else()
    message(STATUS "Thrift or Boost headers not found: anch-codec-thrift is not built")
  endif()
endif()
set(ANCH_CODEC_THRIFT_ENABLED ${ANCH_CODEC_THRIFT_ENABLED} PARENT_SCOPE)
# Plugins -

# Available codecs (codec/all.hpp) +
list(APPEND codec_headers "codec/text.hpp")
list(APPEND codec_types "anch::codec::Text")
set(ANCH_CODEC_ALL_INCLUDES)
foreach(header IN LISTS codec_headers)
  string(APPEND ANCH_CODEC_ALL_INCLUDES "#include \"${header}\"\n")
endforeach()
set(ANCH_CODEC_ALL_MACROS)
foreach(macro IN LISTS codec_macros)
  string(APPEND ANCH_CODEC_ALL_MACROS "/*! Plugin built with the framework */\n#define ${macro}\n")
endforeach()
list(JOIN codec_types ", " ANCH_CODEC_ALL_TYPES)
set(all_dir ${CMAKE_CURRENT_BINARY_DIR}/all/inc)
configure_file(all.hpp.in ${all_dir}/codec/all.hpp @ONLY)
set(all_deps anch::codec)
foreach(plugin IN ITEMS json csv protobuf thrift)
  if(TARGET anch::codec-${plugin})
    list(APPEND all_deps anch::codec-${plugin})
  endif()
endforeach()
anch_add_library(anch-codec-all HEADER_ONLY
  INCLUDE_DIRS ${all_dir}
  PUBLIC_DEPS ${all_deps})
# Available codecs (codec/all.hpp) -

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
