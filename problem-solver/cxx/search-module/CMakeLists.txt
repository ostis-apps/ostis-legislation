file(GLOB SOURCES CONFIGURE_DEPENDS
    "*.cpp" "*.hpp"
    "agents/*.cpp" "agents/*.hpp"
    "keynodes/*.hpp"
)

add_library(search-module SHARED ${SOURCES}
        ../search-module/SearchModule.cpp)
target_link_libraries(search-module
    LINK_PUBLIC sc-machine::sc-memory
    LINK_PUBLIC sc-machine::sc-agents-common
)
target_include_directories(search-module
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
set_target_properties(search-module PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)

if(${SC_CLANG_FORMAT_CODE})
    target_clangformat_setup(search-module)
endif()
