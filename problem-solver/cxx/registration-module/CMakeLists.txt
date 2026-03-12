file(GLOB SOURCES CONFIGURE_DEPENDS
        "keynodes/*.hpp"
        "agent/VerificationAgent.cpp" "agent/VerificationAgent.hpp"
        "agent/registration_agent.cpp" "agent/registration_agent.hpp"
        "agent/AuthenticationAgent.cpp" "agent/AuthenticationAgent.hpp"
)


add_library(registration-module SHARED
        RegistrationModule.cpp
        ${SOURCES})

find_package(OpenSSL REQUIRED)

target_link_libraries(registration-module
        LINK_PUBLIC sc-machine::sc-memory
        LINK_PUBLIC sc-machine::sc-agents-common
        LINK_PUBLIC OpenSSL::SSL
        LINK_PUBLIC OpenSSL::Crypto
)

target_include_directories(registration-module
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC ${OPENSSL_INCLUDE_DIR}
)

set_target_properties(registration-module PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/extensions)
