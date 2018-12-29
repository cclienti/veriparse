set(GOOGLETEST_GIT_PREFIX_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/googletest)
set(GOOGLETEST_GIT_INSTALL_DIR ${GOOGLETEST_GIT_PREFIX_DIR}/install)

message(STATUS "External project google test installation in ${GOOGLETEST_GIT_INSTALL_DIR}")

ExternalProject_Add(googletest-git
    BUILD_IN_SOURCE   1
    PREFIX             ${GOOGLETEST_GIT_PREFIX_DIR}
    GIT_REPOSITORY    https://github.com/google/googletest.git
    GIT_TAG           ec44c6c
    CMAKE_ARGS        -DCMAKE_INSTALL_PREFIX=${GOOGLETEST_GIT_INSTALL_DIR} -DCMAKE_BUILD_TYPE=Release
    UPDATE_COMMAND    ""
)

set_directory_properties(PROPERTY CLEAN_NO_CUSTOM ${GOOGLETEST_GIT_PREFIX_DIR})
