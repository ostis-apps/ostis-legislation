file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agents/*.cpp" "agents/*.hpp"
    "keynodes/*.hpp"
)

add_library(editMessage-module SHARED ${SOURCES})

target_link_libraries(editMessage-module
    LINK_PUBLIC sc-machine::sc-memory
)

target_include_directories(editMessage-module
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(editMessage-module
    PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions
)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(editMessage-module)
endif()