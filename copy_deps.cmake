file(GET_RUNTIME_DEPENDENCIES
  EXECUTABLES "build/build"
  RESOLVED_DEPENDENCIES_VAR found_deps
  UNRESOLVED_DEPENDENCIES_VAR unfound_deps)
if (unfound_deps)
  # not all required libraries found, probably error
endif ()

foreach (found_dep IN LISTS found_deps)
  # Determine if the dep library should be copied.
  if (dep_not_wanted)
    continue()
  endif ()
  configure_file("${found_dep}" "${dest_libdir}" COPY_ONLY)
endforeach ()

file(TOUCH "${tagfile}")
