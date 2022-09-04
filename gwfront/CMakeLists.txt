SET (TARGET_NAME gwfront)


SET (HEADERS
    "src/handler/interface.hpp"
    "src/handler/json_checker.hpp"
    "src/handler/postgres_writer.hpp"
    "src/handler/universal_handler.hpp"
    "src/http/server.hpp"
    "src/http/session.hpp"
    "src/utils/config.hpp"
)

SET (SOURCES
    "src/main.cpp"
    "src/handler/json_checker.cpp"
    "src/handler/postgres_writer.cpp"
    "src/http/server.cpp"
    "src/http/session.cpp"
    "src/utils/config.cpp"
)

ADD_EXECUTABLE (${TARGET_NAME}
    ${SOURCES}
)


TARGET_INCLUDE_DIRECTORIES (${TARGET_NAME}
    PUBLIC "src/"
)

TARGET_LINK_LIBRARIES (${TARGET_NAME}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${PQXX_LIBRARY}
    common
    nlohmann_json
    tomlplusplus
)


INSTALL (
    TARGETS ${TARGET_NAME}
    RUNTIME DESTINATION "${CMAKE_BINARY_DIR}/.."
)