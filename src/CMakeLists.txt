include_directories(${PROJECT_SOURCE_DIR}/include)

option(ENABLE_IO_THREAD "Start up a separate I/O thread, otherwise I'd need to call an update function" ON)
option(USE_STATIC_CRT "Use /MT[d] for dynamic library" OFF)
option(WARNINGS_AS_ERRORS "When enabled, compiles with `-Werror` (on *nix platforms)." OFF)

set(CMAKE_CXX_STANDARD 14)

set(BASE_RPC_SRC
    ${PROJECT_SOURCE_DIR}/include/discord_rpc.h
    discord_rpc.cpp
    ${PROJECT_SOURCE_DIR}/include/discord_register.h
    rpc_connection.h
    rpc_connection.cpp
    serialization.h
    serialization.cpp
    connection.h
    backoff.h
    msg_queue.h
)

if (${BUILD_SHARED_LIBS})
    if (WIN32)
        set(BASE_RPC_SRC ${BASE_RPC_SRC} dllmain.cpp)
    endif ()
endif ()

if (WIN32)
    add_definitions(-DDISCORD_WINDOWS)
    set(BASE_RPC_SRC ${BASE_RPC_SRC} connection_win.cpp discord_register_win.cpp)
    add_library(discord-rpc ${BASE_RPC_SRC})

    if (MSVC)
        if (USE_STATIC_CRT)
            foreach (CompilerFlag
                CMAKE_CXX_FLAGS
                CMAKE_CXX_FLAGS_DEBUG
                CMAKE_CXX_FLAGS_RELEASE
                CMAKE_C_FLAGS
                CMAKE_C_FLAGS_DEBUG
                CMAKE_C_FLAGS_RELEASE)
                string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
            endforeach ()
        endif ()
        
        target_compile_options(discord-rpc PRIVATE
            /EHsc
            /Wall
            /wd4100
            /wd4514
            /wd4625
            /wd5026
            /wd4626
            /wd4668
            /wd4710
            /wd4711
            /wd4820
            /wd4946
            /wd5027
        )
    endif ()
    
    target_link_libraries(discord-rpc PRIVATE psapi advapi32)
endif ()

if (UNIX)
    set(BASE_RPC_SRC ${BASE_RPC_SRC} connection_unix.cpp)

    if (APPLE)
        add_definitions(-DDISCORD_OSX)
        set(BASE_RPC_SRC ${BASE_RPC_SRC} discord_register_osx.m)
    else ()
        add_definitions(-DDISCORD_LINUX)
        set(BASE_RPC_SRC ${BASE_RPC_SRC} discord_register_linux.cpp)
    endif ()

    add_library(discord-rpc ${BASE_RPC_SRC})
    target_link_libraries(discord-rpc PUBLIC pthread)

    if (APPLE)
        target_link_libraries(discord-rpc PRIVATE "-framework AppKit")
    endif ()

    target_compile_options(discord-rpc PRIVATE
        -g
        -Wall
        -Wextra
        -Wpedantic
    )

    if (${WARNINGS_AS_ERRORS})
        target_compile_options(discord-rpc PRIVATE -Werror)
    endif ()

    target_compile_options(discord-rpc PRIVATE
        -Wno-unknown-pragmas
        -Wno-old-style-cast
        -Wno-c++98-compat
        -Wno-c++98-compat-pedantic
        -Wno-missing-noreturn
        -Wno-padded
        -Wno-covered-switch-default
        -Wno-exit-time-destructors
        -Wno-global-constructors
    )

    if (${BUILD_SHARED_LIBS})
        target_compile_options(discord-rpc PRIVATE -fPIC)
    endif ()

    if (APPLE)
        target_link_libraries(discord-rpc PRIVATE "-framework AppKit")
    endif ()
endif ()

target_include_directories(discord-rpc PRIVATE ${RAPIDJSON}/include)

if (NOT ${ENABLE_IO_THREAD})
    target_compile_definitions(discord-rpc PUBLIC -DDISCORD_DISABLE_IO_THREAD)
endif ()

if (${BUILD_SHARED_LIBS})
    target_compile_definitions(discord-rpc PUBLIC -DDISCORD_DYNAMIC_LIB)
    target_compile_definitions(discord-rpc PRIVATE -DDISCORD_BUILDING_SDK)
endif ()

if (CLANG_FORMAT_CMD)
    add_dependencies(discord-rpc clangformat)
endif ()

install(
    TARGETS discord-rpc
    EXPORT "discord-rpc"
    RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
    LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INCLUDES DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(
    FILES
        "../include/discord_rpc.h"
        "../include/discord_register.h"
    DESTINATION "include"
)
