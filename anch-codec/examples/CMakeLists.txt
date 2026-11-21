set_property(DIRECTORY PROPERTY LABELS codec)

if(ANCH_CODEC_JSON_ENABLED)
  anch_add_example(codec SOURCES codec.cpp LIBS anch::codec-all RUN)
endif()
