CMAKE_MINIMUM_REQUIRED (VERSION 3.13.0 FATAL_ERROR)

PROJECT (gateway VERSION 0.1.0 LANGUAGES CXX)

SET (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

INCLUDE (BuildFlags)


SET (Boost_USE_STATIC_LIBS ON)
FIND_PACKAGE (Boost COMPONENTS
    log
    log_setup
    program_options
    REQUIRED
)
ADD_COMPILE_DEFINITIONS ("BOOST_BEAST_USE_STD_STRING_VIEW")


SET (HEADERS
    "src/handler/interface.hpp"
    "src/http/server.hpp"
    "src/http/session.hpp"
    "src/utils/config.hpp"
    "src/utils/log.hpp"
)

SET (SOURCES
    "src/main.cpp"
    "src/http/server.cpp"
    "src/http/session.cpp"
    "src/utils/config.cpp"
    "src/utils/log.cpp"
)

ADD_EXECUTABLE (${PROJECT_NAME}
    ${SOURCES}
)

TARGET_INCLUDE_DIRECTORIES (${PROJECT_NAME}
    PUBLIC "src/"
)

TARGET_LINK_LIBRARIES (${PROJECT_NAME}
    ${Boost_LOG_LIBRARY}
    ${Boost_LOG_SETUP_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    tomlplusplus
)


ADD_SUBDIRECTORY (3rdparty)
