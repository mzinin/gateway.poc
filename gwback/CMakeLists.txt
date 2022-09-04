SET (TARGET_NAME gwback)


SET (HEADERS
    "src/consumer/consumer.hpp"
    "src/message/message.hpp"
    "src/producer/interface.hpp"
    "src/producer/postgres_producer.hpp"
    "src/utils/config.hpp"
)

SET (SOURCES
    "src/main.cpp"
    "src/consumer/consumer.cpp"
    "src/producer/postgres_producer.cpp"
    "src/utils/config.cpp"
)

ADD_EXECUTABLE (${TARGET_NAME}
    ${SOURCES}
)


TARGET_INCLUDE_DIRECTORIES (${TARGET_NAME}
    PUBLIC "src/"
)

TARGET_LINK_LIBRARIES (${TARGET_NAME}
    ${Boost_LOG_LIBRARY}
    ${Boost_LOG_SETUP_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${PQXX_LIBRARY}
    common
    tomlplusplus
)


INSTALL (
    TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION "${CMAKE_BINARY_DIR}/.."
)