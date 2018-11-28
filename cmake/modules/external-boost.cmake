set(BOOST_URL_PREFIX_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/boost)
set(BOOST_URL_INSTALL_DIR ${BOOST_URL_PREFIX_DIR}/install)

message(STATUS "External project Boost installation in ${BOOST_URL_INSTALL_DIR}")

set(BOOST_URL            https://kent.dl.sourceforge.net/project/boost/boost/1.68.0/boost_1_68_0.tar.bz2)
set(BOOST_SHA256         7f6130bc3cf65f56a618888ce9d5ea704fa10b462be126ad053e80e553d6d8b7)

# Patch command is used to setup the install
set(BOOST_PATCH_CMD      echo "using gcc : : ${CMAKE_CXX_COMPILER} : <cxxflags>-Wno-deprecated-declarations $<SEMICOLON>\\n" > tools/build/src/user-config.jam)
set(BOOST_BOOTSTRAP_CMD  ./bootstrap.sh --prefix=${BOOST_URL_INSTALL_DIR})
set(BOOST_BUILD_CMD      ./b2 --prefix=${BOOST_URL_INSTALL_DIR} -j12 variant=release install)

# Boost patch if needed (boost 1.64)
# Installed command is used to patch boost install against -Werror=parentheses
# get_filename_component(BOOST_PATCH_FOR_INSTALLED_HEADER ${CMAKE_CURRENT_LIST_DIR}/patches/boost-wno-parentheses.patch REALPATH)
# set(BOOST_INSTALL_CMD patch --ignore-whitespace -d ${BOOST_URL_INSTALL_DIR}/include boost/mpl/assert.hpp ${BOOST_PATCH_FOR_INSTALLED_HEADER})

set(BOOST_LIBRARY_PREFIX_PATH ${BOOST_URL_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX})

foreach(BOOST_MOD ${BOOST_MODULES})
  set(BOOST_LIB boost_${BOOST_MOD})
  get_filename_component(${BOOST_LIB}_PATH ${BOOST_LIBRARY_PREFIX_PATH}${BOOST_LIB}${CMAKE_STATIC_LIBRARY_SUFFIX} REALPATH)
  set(BOOST_PATH_LIBRARIES ${BOOST_PATH_LIBRARIES} ${${BOOST_LIB}_PATH})
endforeach()

ExternalProject_Add(boost-url
    BUILD_IN_SOURCE   1
    PREFIX            ${BOOST_URL_PREFIX_DIR}
    URL               ${BOOST_URL}
    URL_HASH          SHA256=${BOOST_SHA256}
    UPDATE_COMMAND    ""
    PATCH_COMMAND     ${BOOST_PATCH_CMD}
    CONFIGURE_COMMAND ${BOOST_BOOTSTRAP_CMD}
    BUILD_COMMAND     ${BOOST_BUILD_CMD}
    INSTALL_COMMAND   "" # ${BOOST_INSTALL_CMD} # For boost patch
    BUILD_BYPRODUCTS  ${BOOST_PATH_LIBRARIES}
)

set_directory_properties(PROPERTY CLEAN_NO_CUSTOM ${BOOST_URL_PREFIX_DIR})

# Include directory
set(BOOST_INCLUDE_DIR ${BOOST_URL_INSTALL_DIR}/include)

# Lib declaration
foreach(BOOST_MOD ${BOOST_MODULES})
  set(BOOST_LIB boost_${BOOST_MOD})
  set(BOOST_LIBRARIES ${BOOST_LIBRARIES} ${BOOST_LIB})

  get_filename_component(${BOOST_LIB}_PATH
    ${BOOST_LIBRARY_PREFIX_PATH}${BOOST_LIB}${CMAKE_STATIC_LIBRARY_SUFFIX}
    REALPATH)

  add_library(${BOOST_LIB} STATIC IMPORTED GLOBAL)
  add_dependencies(${BOOST_LIB} boost-url)
  set_property(TARGET ${BOOST_LIB} PROPERTY IMPORTED_LOCATION ${${BOOST_LIB}_PATH})
endforeach()

message(STATUS "Boost libraries: ${BOOST_LIBRARIES}")
