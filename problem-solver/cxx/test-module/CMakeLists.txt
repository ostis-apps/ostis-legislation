file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agents/*.cpp" "agents/*.hpp"
    "keynodes/*.hpp"
    "models/*.cpp" "models/*.hpp"
)

add_library(test-module SHARED ${SOURCES})
target_link_libraries(test-module
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
)
target_include_directories(test-module
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(test-module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(test-module)
endif()
