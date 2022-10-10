INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/dialogextractorprocess.h \
    $$PWD/dialogxextractor.h \
    $$PWD/xextractorwidget.h

SOURCES += \
    $$PWD/dialogextractorprocess.cpp \
    $$PWD/dialogxextractor.cpp \
    $$PWD/xextractorwidget.cpp

FORMS += \
    $$PWD/dialogxextractor.ui \
    $$PWD/xextractorwidget.ui

!contains(XCONFIG, xshortcuts) {
    XCONFIG += xshortcuts
    include($$PWD/../XShortcuts/xshortcuts.pri)
}

!contains(XCONFIG, xextractor) {
    XCONFIG += xextractor
    include($$PWD/../XExtractor/xextractor.pri)
}

DISTFILES += \
    $$PWD/LICENSE \
    $$PWD/README.md \
    $$PWD/xextractorwidget.cmake
