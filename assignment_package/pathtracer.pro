QT += core widgets multimedia

TARGET = 277
TEMPLATE = app
CONFIG += c++11
CONFIG += warn_on
CONFIG += debug

INCLUDEPATH += include

include(src/src.pri)

FORMS += forms/mainwindow.ui \
    forms/cameracontrolshelp.ui

RESOURCES += glsl.qrc \
    sfx.qrc


# Enable console on Windows so that we can actually see debug prints.
win32 {
    CONFIG += console
}

macx {
    # Steps to get this to compile after installing Xcode 7
    # Add the line below (or the macosx10.11)
    # I added the macos 10.10 sdk into Xcode 7's developer sdk folder
    # I changed the Command Line Tool option in Xcode 6 and 7 (Preferences --> Locations --> Command Line Tool) from blank to Xcode 7.2
    QMAKE_MAC_SDK = macosx10.10
    INCLUDEPATH += /usr/local/include
}

*-clang*|*-g++* {
    message("Enabling additional warnings")
    CONFIG -= warn_on
    QMAKE_CXXFLAGS += -Wall -Wextra -pedantic -Winit-self
    QMAKE_CXXFLAGS += -Wno-strict-aliasing
    QMAKE_CXXFLAGS += -Wno-unneeded-internal-declaration
    QMAKE_CXXFLAGS += -fno-omit-frame-pointer
    INCLUDEPATH += C:/Program Files/boost/boost_1_58_0
}
linux-clang*|linux-g++*|macx-clang*|macx-g++* {
    message("Enabling stack protector")
    QMAKE_CXXFLAGS += -fstack-protector-all
}

# FOR LINUX & MAC USERS INTERESTED IN ADDITIONAL BUILD TOOLS
# ----------------------------------------------------------
# This conditional exists to enable Address Sanitizer (ASAN) during
# the automated build. ASAN is a compiled-in tool which checks for
# memory errors (like Valgrind). You may use these tools yourself;
# `asan-build.sh` builds the project with ASAN. But be aware: ASAN may
# trigger a lot of false-positive leak warnings for the Qt libraries.
# (Use `asan-run.sh` to run the program with leak checking disabled.)
address_sanitizer {
    message("Enabling Address Sanitizer")
    QMAKE_CXXFLAGS += -fsanitize=address
    QMAKE_LFLAGS += -fsanitize=address
}

HEADERS +=

SOURCES +=
