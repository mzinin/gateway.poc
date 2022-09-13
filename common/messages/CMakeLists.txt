SET (TARGET_NAME messages)


ADD_LIBRARY (${TARGET_NAME}
    INTERFACE
)

TARGET_INCLUDE_DIRECTORIES (${TARGET_NAME}
    INTERFACE "${CMAKE_CURRENT_BINARY_DIR}/../.."
)

TARGET_LINK_LIBRARIES (${TARGET_NAME}
    INTERFACE flatbuffers
)


FILE (GLOB SCHEMA_FILES "*.fbs")
SET (GENERATED_FILE_EXTENSION "hpp")

FOREACH (SCHEMA_FILE ${SCHEMA_FILES})
    CMAKE_PATH (GET SCHEMA_FILE STEM SCHEMA_FILE_BASENAME)
    SET (GENERATED_FILE_BASENAME "${SCHEMA_FILE_BASENAME}.${GENERATED_FILE_EXTENSION}")
    CMAKE_PATH (APPEND CMAKE_CURRENT_BINARY_DIR "${GENERATED_FILE_BASENAME}" OUTPUT_VARIABLE GENERATED_FILE)

    ADD_CUSTOM_TARGET (${GENERATED_FILE_BASENAME}
        DEPENDS "${GENERATED_FILE}"
    )

    ADD_CUSTOM_COMMAND (
        OUTPUT ${GENERATED_FILE}
        # generate header file
        COMMAND $<TARGET_FILE:flatc> ARGS
            "-o" "${CMAKE_CURRENT_BINARY_DIR}"
            "--cpp"
            "--scoped-enums"
            "--cpp-std" "c++17"
            "--filename-suffix" ""
            "--filename-ext" "${GENERATED_FILE_EXTENSION}"
            "${SCHEMA_FILE}"
        # wrap flatbuffers include into pragmas to avoid "comparing floating-point with ‘==’" warnings
        COMMAND sed ARGS
            "-i"
            "/^#include \"flatbuffers\\/flatbuffers.h\"$/i #pragma GCC diagnostic push\\n#pragma GCC diagnostic ignored \"-Wfloat-equal\""
            "${GENERATED_FILE}"
        COMMAND sed ARGS
            "-i"
            "/^#include \"flatbuffers\\/flatbuffers.h\"$/a #pragma GCC diagnostic pop"
            "${GENERATED_FILE}"
        DEPENDS "${SCHEMA_FILE}"
        COMMENT "Processing schema file: ${SCHEMA_FILE}"
        VERBATIM
    )

    ADD_DEPENDENCIES (${TARGET_NAME} ${GENERATED_FILE_BASENAME})
ENDFOREACH ()
