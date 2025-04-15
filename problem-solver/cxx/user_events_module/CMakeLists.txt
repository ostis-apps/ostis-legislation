file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agent/*.cpp" "agent/*.hpp"
    "keynodes/*.hpp"
)

add_library(user-events-module SHARED ${SOURCES})
target_link_libraries(user-events-module
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
)
target_include_directories(user-events-module
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(user-events-module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(user-events-module)
endif()