cmake_minimum_required(VERSION 3.16)
project(GUI)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /subsystem:windows")
add_definitions(-DUNICODE -D_UNICODE)
add_executable(GUI
        main.cpp
        LoginWindow.cpp
        RegistrationWindow.cpp
        ActivateWindow.cpp
        PipeUtils.cpp
        MainWindow.cpp
        TrayIcon.h
        TrayIcon.cpp
)

target_link_libraries(GUI PRIVATE
        user32.lib
        gdi32.lib
        kernel32.lib
        winspool.lib
        shell32.lib
        ole32.lib
        oleaut32.lib
        uuid.lib
        comdlg32.lib
        advapi32.lib
)
