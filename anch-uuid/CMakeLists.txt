# AnCH UUID: UUID generation and parsing
# Random bytes source: arc4random_buf (fast, fork safe ; glibc >= 2.36, BSD, macOS) or getentropy (POSIX) +
include(CheckCXXSymbolExists)
check_cxx_symbol_exists(arc4random_buf "cstdlib" ANCH_UUID_HAVE_ARC4RANDOM)
set(uuid_private_definitions)
if(ANCH_UUID_HAVE_ARC4RANDOM)
  list(APPEND uuid_private_definitions ANCH_UUID_ARC4RANDOM)
endif()
# Random bytes source: arc4random_buf (fast, fork safe ; glibc >= 2.36, BSD, macOS) or getentropy (POSIX) -

anch_add_library(anch-uuid
  PRIVATE_DEFINITIONS ${uuid_private_definitions})

if(ANCH_BUILD_TESTS)
  add_subdirectory(test)
endif()
if(ANCH_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
