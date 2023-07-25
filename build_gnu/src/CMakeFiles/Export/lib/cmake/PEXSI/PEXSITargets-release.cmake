#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "PEXSI::PEXSI" for configuration "Release"
set_property(TARGET PEXSI::PEXSI APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PEXSI::PEXSI PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C;CXX;Fortran"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libpexsi.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS PEXSI::PEXSI )
list(APPEND _IMPORT_CHECK_FILES_FOR_PEXSI::PEXSI "${_IMPORT_PREFIX}/lib/libpexsi.a" )

# Import target "PEXSI::superlu_dist" for configuration "Release"
set_property(TARGET PEXSI::superlu_dist APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PEXSI::superlu_dist PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsuperlu_dist.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS PEXSI::superlu_dist )
list(APPEND _IMPORT_CHECK_FILES_FOR_PEXSI::superlu_dist "${_IMPORT_PREFIX}/lib/libsuperlu_dist.a" )

# Import target "PEXSI::parmetis" for configuration "Release"
set_property(TARGET PEXSI::parmetis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PEXSI::parmetis PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libparmetis.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS PEXSI::parmetis )
list(APPEND _IMPORT_CHECK_FILES_FOR_PEXSI::parmetis "${_IMPORT_PREFIX}/lib/libparmetis.a" )

# Import target "PEXSI::metis" for configuration "Release"
set_property(TARGET PEXSI::metis APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(PEXSI::metis PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "C"
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libmetis.a"
  )

list(APPEND _IMPORT_CHECK_TARGETS PEXSI::metis )
list(APPEND _IMPORT_CHECK_FILES_FOR_PEXSI::metis "${_IMPORT_PREFIX}/lib/libmetis.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
