include_directories(${CMAKE_CURRENT_LIST_DIR})

if (NOT DEFINED XEXTRACTOR_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../XExtractor/xextractor.cmake)
    set(XEXTRACTORWIDGET_SOURCES ${XEXTRACTORWIDGET_SOURCES} ${XEXTRACTOR_SOURCES})
endif()
if (NOT DEFINED XDIALOGPROCESS_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/xdialogprocess.cmake)
    set(XEXTRACTORWIDGET_SOURCES ${XEXTRACTORWIDGET_SOURCES} ${XDIALOGPROCESS_SOURCES})
endif()
if (NOT DEFINED XDIALOGPROCESS_SOURCES)
    include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogdump.cmake)
    set(XEXTRACTORWIDGET_SOURCES ${DIALOGDUMP_SOURCES} ${DIALOGDUMP_SOURCES})
endif()

include(${CMAKE_CURRENT_LIST_DIR}/../Controls/xcomboboxex.cmake)

set(XEXTRACTORWIDGET_SOURCES
    ${XEXTRACTORWIDGET_SOURCES}
    ${XEXTRACTOR_SOURCES}
    ${XDIALOGPROCESS_SOURCES}
    ${DIALOGDUMP_SOURCES}
    ${XCOMBOBOXEX_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xextractorwidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xextractorwidget.h
    ${CMAKE_CURRENT_LIST_DIR}/xextractorwidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogxextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogxextractor.h
    ${CMAKE_CURRENT_LIST_DIR}/dialogxextractor.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogextractorprocess.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogextractorprocess.h
)
