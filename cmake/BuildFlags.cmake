IF (NOT CMAKE_BUILD_TYPE)
    SET (CMAKE_BUILD_TYPE RELEASE)
ENDIF ()

MESSAGE ("-- Build type: ${CMAKE_BUILD_TYPE}")

IF (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    ADD_COMPILE_OPTIONS (-std=c++20)

    ADD_COMPILE_OPTIONS (-Wall)
    ADD_COMPILE_OPTIONS (-Wcast-align)
    ADD_COMPILE_OPTIONS (-Wcast-qual)
    ADD_COMPILE_OPTIONS (-Werror)
    ADD_COMPILE_OPTIONS (-Wextra)
    ADD_COMPILE_OPTIONS (-Wfloat-equal)
    ADD_COMPILE_OPTIONS (-Wpedantic)
    ADD_COMPILE_OPTIONS (-Wredundant-decls)
    ADD_COMPILE_OPTIONS (-Wshadow)
    ADD_COMPILE_OPTIONS (-Wunused)

    SET (CMAKE_CXX_FLAGS_RELEASE "-O3 -funroll-loops")
    SET (CMAKE_CXX_FLAGS_DEBUG   "-O0 -ggdb -g3")

    ADD_LINK_OPTIONS ("-static-libgcc")
    ADD_LINK_OPTIONS ("-static")
ENDIF ()
