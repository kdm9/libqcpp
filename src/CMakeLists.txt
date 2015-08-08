ADD_SUBDIRECTORY(tests)
ADD_SUBDIRECTORY(util)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/qc-config.hh.in
               ${CMAKE_BINARY_DIR}/qc-config.hh)

# Library
SET(QCPP_HEADERS
        qcpp.hh
        qc-io.hh
        qc-processor.hh
        qc-length.hh
        qc-gbs.hh
        qc-adaptor.hh
        qc-measure.hh
        qc-qualtrim.hh
        )

ADD_LIBRARY(libqc++
        qc-io.cc
        qc-processor.cc
        qc-length.cc
        qc-gbs.cc
        qc-adaptor.cc
        qc-measure.cc
        qc-qualtrim.cc
        ext/ssw.c
        ext/ssw_cpp.cpp
        )

SET_TARGET_PROPERTIES(libqc++ PROPERTIES OUTPUT_NAME qc++)
TARGET_LINK_LIBRARIES(libqc++ ${DEPENDS_LIBS})

INSTALL(TARGETS libqc++ DESTINATION "lib")
INSTALL(FILES ${QCPP_HEADERS} DESTINATION "include")
