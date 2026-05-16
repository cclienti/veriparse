# FindGMP.cmake - Find the GMP and GMPXX libraries
#
# This module defines:
#   GMP::GMP   - imported target for libgmp
#   GMP::GMPXX - imported target for libgmpxx
#   GMP_FOUND

include(FindPackageHandleStandardArgs)

find_path(GMP_INCLUDE_DIR NAMES gmp.h)
find_library(GMP_LIBRARY NAMES gmp)
find_path(GMPXX_INCLUDE_DIR NAMES gmpxx.h)
find_library(GMPXX_LIBRARY NAMES gmpxx)

find_package_handle_standard_args(GMP
  REQUIRED_VARS GMP_LIBRARY GMP_INCLUDE_DIR GMPXX_LIBRARY GMPXX_INCLUDE_DIR)
mark_as_advanced(GMP_INCLUDE_DIR GMP_LIBRARY GMPXX_INCLUDE_DIR GMPXX_LIBRARY)

if(GMP_FOUND)
  if(NOT TARGET GMP::GMP)
    add_library(GMP::GMP UNKNOWN IMPORTED)
    set_target_properties(GMP::GMP PROPERTIES
      IMPORTED_LOCATION "${GMP_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GMP_INCLUDE_DIR}")
  endif()

  if(NOT TARGET GMP::GMPXX)
    add_library(GMP::GMPXX UNKNOWN IMPORTED)
    set_target_properties(GMP::GMPXX PROPERTIES
      IMPORTED_LOCATION "${GMPXX_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GMPXX_INCLUDE_DIR}")
  endif()
endif()
