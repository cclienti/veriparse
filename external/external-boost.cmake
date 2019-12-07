if(NOT VERIPARSE_PROCESSOR_COUNT)
  set(VERIPARSE_PROCESSOR_COUNT 1)
  message(STATUS "VERIPARSE_PROCESSOR_COUNT not set, forcing to 1")
endif()

set(BOOST_URL_PREFIX_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/boost)
set(BOOST_URL_INSTALL_DIR ${BOOST_URL_PREFIX_DIR}/install)

execute_process(COMMAND python3 -c "import sys; sys.stdout.write('{}.{}'.format(sys.version_info.major, sys.version_info.minor))"
                OUTPUT_VARIABLE PYTHON_VERSION)

message(STATUS "Python Major Version: ${PYTHON_VERSION}")
message(STATUS "External project Boost installation in ${BOOST_URL_INSTALL_DIR}")

set(BOOST_URL            https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.bz2)
set(BOOST_SHA256         430ae8354789de4fd19ee52f3b1f739e1fba576f0aded0897c3c2bc00fb38778)

# Patch command is used to setup the install
set(BOOST_PATCH_CMD
  echo "using gcc : : ${CMAKE_CXX_COMPILER} : <cxxflags>-Wno-deprecated-declarations $<SEMICOLON>"  > tools/build/src/user-config.jam &&
  echo "using python : ${PYTHON_VERSION} : /usr/bin/python${PYTHON_VERSION} : /usr/include/python${PYTHON_VERSION}m : /usr/lib $<SEMICOLON>" >> tools/build/src/user-config.jam)
set(BOOST_BOOTSTRAP_CMD  ./bootstrap.sh --prefix=${BOOST_URL_INSTALL_DIR})
set(BOOST_BUILD_CMD      ./b2 -j${VERIPARSE_PROCESSOR_COUNT} python=${PYTHON_VERSION} toolset=gcc link=static threading=multi variant=release install)

# Boost patch if needed (boost 1.64)
# Installed command is used to patch boost install against -Werror=parentheses
# get_filename_component(BOOST_PATCH_FOR_INSTALLED_HEADER ${CMAKE_CURRENT_LIST_DIR}/patches/boost-wno-parentheses.patch REALPATH)
# set(BOOST_INSTALL_CMD patch --ignore-whitespace -d ${BOOST_URL_INSTALL_DIR}/include boost/mpl/assert.hpp ${BOOST_PATCH_FOR_INSTALLED_HEADER})

set(BOOST_LIBRARY_PREFIX_PATH ${BOOST_URL_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX})

ExternalProject_Add(boost-ext
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
