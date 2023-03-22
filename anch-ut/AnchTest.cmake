# Save project's policies
cmake_policy(PUSH)
cmake_policy(SET CMP0057 NEW) # if IN_LIST

function(anch_add_tests)

  if (ARGC LESS 1)
    message(FATAL_ERROR "Unit tests executable is mandatory as first argument")
  endif()

endfunction()

# Restore project's policies
cmake_policy(POP)