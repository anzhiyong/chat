cmake_minimum_required(VERSION 3.16)
project(QtChatClient LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_executable(QtChatClient
    main.cpp
    ChatClient.h
    ChatClient.cpp
    MainWindow.h
    MainWindow.cpp
    MainWindow.ui
    LeftBar.h
    LeftBar.cpp
    LeftBar.ui
    SessionPanel.h
    SessionPanel.cpp
    SessionPanel.ui
    SessionItemWidget.h
    SessionItemWidget.cpp
    SessionItemWidget.ui
    ChatPanel.h
    ChatPanel.cpp
    ChatPanel.ui
)

target_link_libraries(QtChatClient PRIVATE Qt6::Widgets Qt6::Network)

if (WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt6::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    get_filename_component(QT_ROOT_DIR "${QT_BIN_DIR}" DIRECTORY)
    set(QT_PLATFORMS_DIR "${QT_ROOT_DIR}/plugins/platforms")
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    add_custom_command(TARGET QtChatClient POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:QtChatClient>/platforms"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT_BIN_DIR}/Qt6Core.dll"
            "${QT_BIN_DIR}/Qt6Gui.dll"
            "${QT_BIN_DIR}/Qt6Widgets.dll"
            "${QT_BIN_DIR}/Qt6Network.dll"
            "$<TARGET_FILE_DIR:QtChatClient>"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${QT_PLATFORMS_DIR}/qwindows.dll"
            "$<TARGET_FILE_DIR:QtChatClient>/platforms"
        COMMENT "Deploying Qt runtime for QtChatClient"
    )

    if (MINGW)
        add_custom_command(TARGET QtChatClient POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_BIN_DIR}/libstdc++-6.dll"
                "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
                "${MINGW_BIN_DIR}/libwinpthread-1.dll"
                "$<TARGET_FILE_DIR:QtChatClient>"
            COMMENT "Deploying MinGW runtime for QtChatClient"
        )
    endif ()
endif ()
