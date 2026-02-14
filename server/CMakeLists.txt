cmake_minimum_required(VERSION 3.16)
project(QtChatServer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network)

add_executable(QtChatServer
    main.cpp
    AuthRegistry.h
    AuthRegistry.cpp
    FriendRegistry.h
    FriendRegistry.cpp
    GroupRegistry.h
    GroupRegistry.cpp
    ChatServer.h
    ChatServer.cpp
    ChatServerHandlers.cpp
    MessageStore.h
    MessageStore.cpp
)

target_link_libraries(QtChatServer PRIVATE Qt6::Core Qt6::Network)

if (WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    add_custom_command(TARGET QtChatServer POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT_BIN_DIR}/Qt6Core.dll"
            "${QT_BIN_DIR}/Qt6Network.dll"
            "$<TARGET_FILE_DIR:QtChatServer>"
        COMMENT "Deploying Qt runtime for QtChatServer"
    )

    if (MINGW)
        add_custom_command(TARGET QtChatServer POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_BIN_DIR}/libstdc++-6.dll"
                "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
                "${MINGW_BIN_DIR}/libwinpthread-1.dll"
                "$<TARGET_FILE_DIR:QtChatServer>"
            COMMENT "Deploying MinGW runtime for QtChatServer"
        )
    endif ()
endif ()
