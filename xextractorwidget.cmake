include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../XExtractor/xextractor.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/xdialogprocess.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../FormatDialogs/dialogdump.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../Controls/xcomboboxex.cmake)

set(XEXTRACTORWIDGET_SOURCES
    ${XEXTRACTOR_SOURCES}
    ${XDIALOGPROCESS_SOURCES}
    ${DIALOGDUMP_SOURCES}
    ${XCOMBOBOXEX_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xextractorwidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xextractorwidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogxextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogxextractor.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogextractorprocess.cpp
)
