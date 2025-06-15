QT       += concurrent

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogxextractor.h \
    $$PWD/xextractorwidget.h

SOURCES += \
    $$PWD/dialogxextractor.cpp \
    $$PWD/xextractorwidget.cpp

FORMS += \
    $$PWD/dialogxextractor.ui \
    $$PWD/xextractorwidget.ui

!contains(XCONFIG, xextractor) {
    XCONFIG += xextractor
    include($$PWD/../XExtractor/xextractor.pri)
}

!contains(XCONFIG, xdialogprocess) {
    XCONFIG += xdialogprocess
    include($$PWD/../FormatDialogs/xdialogprocess.pri)
}

!contains(XCONFIG, dialogdump) {
    XCONFIG += dialogdump
    include($$PWD/../FormatDialogs/dialogdump.pri)
}

!contains(XCONFIG, xcomboboxex) {
    XCONFIG += xcomboboxex
    include($$PWD/../Controls/xcomboboxex.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xextractorwidget.cmake
