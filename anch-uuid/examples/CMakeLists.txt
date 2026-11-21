set_property(DIRECTORY PROPERTY LABELS uuid)

# Examples use the UUID providers of anch-crypto (versions 3 and 5) and anch-device (version 1)
if(ANCH_BUILD_CRYPTO)
  anch_add_example(uuid SOURCES uuid.cpp LIBS anch::uuid anch::crypto RUN)
  anch_add_example(uuid-providers SOURCES providers.cpp LIBS anch::uuid anch::crypto anch::device RUN)
endif()
