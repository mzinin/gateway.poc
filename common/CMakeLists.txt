SET (TARGET_NAME common)


SET (HEADERS
    "utils/config.hpp"
    "utils/log.hpp"
    "utils/wait_signal.hpp"
)

SET (SOURCES
    "utils/config.cpp"
    "utils/log.cpp"
    "utils/wait_signal.cpp"
)


ADD_LIBRARY (${TARGET_NAME} STATIC
    ${HEADERS}
    ${SOURCES}
)


TARGET_LINK_LIBRARIES (${TARGET_NAME}
    PUBLIC ${Boost_LOG_LIBRARY}
    PUBLIC ${Boost_LOG_SETUP_LIBRARY}
    PUBLIC tomlplusplus
)


TARGET_INCLUDE_DIRECTORIES (${TARGET_NAME}
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/.."
)
