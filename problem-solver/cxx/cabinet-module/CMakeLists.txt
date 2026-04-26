find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

file(GLOB SOURCES CONFIGURE_DEPENDS
        "*.cpp" "*.hpp"
        "agents/*.cpp" "agents/*.hpp"
        "keynodes/*.hpp"
)

add_library(cabinet-module SHARED
        ${SOURCES}
)

target_link_libraries(cabinet-module
        LINK_PUBLIC sc-machine::sc-memory
        LINK_PUBLIC sc-machine::sc-agents-common
        LINK_PUBLIC OpenSSL::SSL
        LINK_PUBLIC OpenSSL::Crypto
)

target_include_directories(cabinet-module
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC ${OPENSSL_INCLUDE_DIR}
)

set_target_properties(cabinet-module PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(cabinet-module)
endif()