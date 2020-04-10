TARGET = sylvan-cli
DESTDIR = $$PWD

include(../lib/lib.pri)
include(../lib/libexport.pri)

SYLVAN_CLI_VERSION = 2.0.0

macx-xcode {
    DEFINES += SYLVAN_CLI_VERSION=\"$$SYLVAN_CLI_VERSION\"
} else {
    OBJECTS_DIR = .obj/
    MOC_DIR = .moc/
    RCC_DIR = .rcc/
    DEFINES += SYLVAN_CLI_VERSION=\\\"$$SYLVAN_CLI_VERSION\\\"
}

win32 {
    CONFIG += console
}

!win32-msvc* {
	QMAKE_CXXFLAGS += -Wextra
}

mac {
    CONFIG -= app_bundle
}

QT = core

# Code
include(src/src.pri)

# Resources
include(res/res.pri)
