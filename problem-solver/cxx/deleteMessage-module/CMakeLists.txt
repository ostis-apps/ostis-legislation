find_package(OpenSSL REQUIRED)

file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agents/*.cpp" "agents/*.hpp"
    "keynodes/*.hpp"
)

add_library(deleteMessage-module SHARED ${SOURCES})

target_link_libraries(deleteMessage-module
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC OpenSSL::SSL
    LINK_PUBLIC OpenSSL::Crypto
)

target_include_directories(deleteMessage-module
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${OPENSSL_INCLUDE_DIR}
)

set_target_properties(deleteMessage-module
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(deleteMessage-module)
endif()
