cmake_minimum_required(VERSION 3.10)
project(DiscordRPC)

include(GNUInstallDirs)

file(GLOB_RECURSE ALL_SOURCE_FILES
    include/*.h
    src/*.cpp src/*.h src/*.c
)

find_program(CLANG_FORMAT_CMD clang-format${CLANG_FORMAT_SUFFIX})

if(CLANG_FORMAT_CMD)
    add_custom_target(
        clangformat
        COMMAND ${CLANG_FORMAT_CMD} -i -style=file -fallback-style=none ${ALL_SOURCE_FILES}
        DEPENDS ${ALL_SOURCE_FILES}
    )
endif()

find_file(RAPIDJSON NAMES rapidjson rapidjson-1.1.0 PATHS ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty CMAKE_FIND_ROOT_PATH_BOTH)
add_library(rapidjson STATIC IMPORTED ${RAPIDJSON})

add_subdirectory(src)
