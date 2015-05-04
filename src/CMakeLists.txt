# Library
SET(QCPP_HEADERS
        qcpp.hh
        qc-io.hh
        qc-processor.hh
        qc-length.hh
        qc-gbs.hh
        qc-adaptor.hh
        )

ADD_LIBRARY(libqc++
        qc-io.cc
        qc-processor.cc
        qc-length.cc
        qc-gbs.cc
        qc-adaptor.cc
        ext/ssw.c
        ext/ssw_cpp.cpp
        )

SET_TARGET_PROPERTIES(libqc++ PROPERTIES OUTPUT_NAME qc++)
TARGET_LINK_LIBRARIES(libqc++ ${DEPENDS_LIBS})

INSTALL(TARGETS libqc++ DESTINATION "lib")
INSTALL(FILES ${QCPP_HEADERS} DESTINATION "include")
