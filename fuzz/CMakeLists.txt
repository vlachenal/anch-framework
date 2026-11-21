# libFuzzer targets (fuzz preset: Clang, libraries built with -fsanitize=fuzzer-no-link,address,undefined)
#   cmake --preset fuzz && cmake --build --preset fuzz
#   build/fuzz/bin/anch-fuzz-json -max_total_time=60 fuzz/corpus/json
if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  message(FATAL_ERROR "ANCH_BUILD_FUZZ needs Clang (libFuzzer)")
endif()

# anch_add_fuzzer(<name> SOURCES <files>... LIBS <targets>...) +
function(anch_add_fuzzer name)
  cmake_parse_arguments(PARSE_ARGV 1 ARG "" "" "SOURCES;LIBS")
  add_executable(anch-fuzz-${name} ${ARG_SOURCES})
  anch_target_build_options(anch-fuzz-${name})
  target_link_libraries(anch-fuzz-${name} PRIVATE ${ARG_LIBS})
  target_compile_options(anch-fuzz-${name} PRIVATE -fsanitize=fuzzer)
  target_link_options(anch-fuzz-${name} PRIVATE -fsanitize=fuzzer)
endfunction()
# anch_add_fuzzer(<name> SOURCES <files>... LIBS <targets>...) -

anch_add_fuzzer(json SOURCES fuzzJson.cpp LIBS anch::json)
anch_add_fuzzer(uuid SOURCES fuzzUuid.cpp LIBS anch::uuid)
anch_add_fuzzer(date SOURCES fuzzDate.cpp LIBS anch::date)
anch_add_fuzzer(ini SOURCES fuzzIni.cpp LIBS anch::ini)
if(ANCH_BUILD_CRYPTO)
  anch_add_fuzzer(base64 SOURCES fuzzBase64.cpp LIBS anch::crypto)
endif()
if(ANCH_BUILD_REST)
  anch_add_fuzzer(multipart SOURCES fuzzMultipart.cpp LIBS anch::rest-common)
  anch_add_fuzzer(form SOURCES fuzzForm.cpp LIBS anch::rest-common)
  anch_add_fuzzer(media-type SOURCES fuzzMediaType.cpp LIBS anch::codec)
endif()
