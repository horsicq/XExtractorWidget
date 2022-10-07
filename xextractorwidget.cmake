include_directories(${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/../XExtractor/xextractor.cmake)

set(XEXTRACTORWIDGET_SOURCES
    ${XEXTRACTOR_SOURCES}
    ${CMAKE_CURRENT_LIST_DIR}/xextractorwidget.cpp
    ${CMAKE_CURRENT_LIST_DIR}/xextractorwidget.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogxextractor.cpp
    ${CMAKE_CURRENT_LIST_DIR}/dialogxextractor.ui
    ${CMAKE_CURRENT_LIST_DIR}/dialogextractorprocess.cpp
)
