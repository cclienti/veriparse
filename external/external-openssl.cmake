if(NOT VERIPARSE_PROCESSOR_COUNT)
  set(VERIPARSE_PROCESSOR_COUNT 1)
  message(STATUS "VERIPARSE_PROCESSOR_COUNT not set, forcing to 1")
endif()

set(OPENSSL_URL_PREFIX_DIR ${CMAKE_CURRENT_BINARY_DIR}/external/openssl)
set(OPENSSL_URL_INSTALL_DIR ${OPENSSL_URL_PREFIX_DIR}/install)

message(STATUS "External project openssl installation in ${OPENSSL_URL_INSTALL_DIR}")

ExternalProject_Add(openssl-ext
    PREFIX            ${CMAKE_CURRENT_BINARY_DIR}/external/openssl
    URL               https://www.openssl.org/source/openssl-1.1.1a.tar.gz
    URL_HASH          SHA256=fc20130f8b7cbd2fb918b2f14e2f429e109c31ddd0fb38fc5d71d9ffed3f9f41
    UPDATE_COMMAND    ""
    PATCH_COMMAND     ""
    CONFIGURE_COMMAND ../openssl-ext/config --prefix=${OPENSSL_URL_INSTALL_DIR}
                                            --openssldir=${OPENSSL_URL_INSTALL_DIR}
    BUILD_COMMAND     make -j${VERIPARSE_PROCESSOR_COUNT}
    INSTALL_COMMAND   make install_sw
)

set_directory_properties(PROPERTY CLEAN_NO_CUSTOM ${OPENSSL_URL_PREFIX_DIR})
