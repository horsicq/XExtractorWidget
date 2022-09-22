INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

FORMS += \
    $$PWD/dialogxextractor.ui \
    $$PWD/xextractorwidget.ui

HEADERS += \
    $$PWD/dialogxextractor.h \
    $$PWD/xextractorwidget.h

SOURCES += \
    $$PWD/dialogxextractor.cpp \
    $$PWD/xextractorwidget.cpp

!contains(XCONFIG, xshortcuts) {
    XCONFIG += xshortcuts
    include($$PWD/../XShortcuts/xshortcuts.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xextractorwidget.cmake
