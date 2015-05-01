# Library
SET(QCPP_HEADERS
        qcpp.hh
        qc-io.hh
        qc-processor.hh
        qc-length.hh
        )

ADD_LIBRARY(libqc++
        qc-io.cc
        qc-processor.cc
        qc-length.cc
        )

SET_TARGET_PROPERTIES(libqc++ PROPERTIES OUTPUT_NAME qc++)
TARGET_LINK_LIBRARIES(libqc++ ${DEPENDS_LIBS})

INSTALL(TARGETS libqc++ DESTINATION "lib")
INSTALL(FILES ${QCPP_HEADERS} DESTINATION "include")
