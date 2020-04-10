macx {
    TARGET = "Sylvan"
} else {
    TARGET = sylvan
}

DESTDIR = $$PWD

include(../lib/lib.pri)
include(../lib/libexport.pri)

SYLVAN_VERSION = 2.0.0

macx-xcode {
    DEFINES += SYLVAN_VERSION=\"$$SYLVAN_VERSION\"
}else {
    OBJECTS_DIR = .obj/
    MOC_DIR = .moc/
    RCC_DIR = .rcc/
    DEFINES += SYLVAN_VERSION=\\\"$$SYLVAN_VERSION\\\"
}

QT += svg widgets concurrent printsupport sql

win32 {
    CONFIG(debug, debug|release) {
	CONFIG += console
    }
    RC_FILE = res/icons/sylvan-win.rc
}

!win32-msvc* {
	QMAKE_CXXFLAGS += -Wextra
}

macx {
    ICON = res/icons/sylvan_mac.icns
}

UI_HEADERS_DIR = src

include(src/src.pri)
include(ui/ui.pri)
include(res/res.pri)
include(3rdparty/qcustomplot/qcustomplot.pri)

CONFIG(debug, debug|release) {
    include(3rdparty/modeltest/modeltest.pri)
}
