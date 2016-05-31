INCLUDE_DIRECTORIES(processors)
IF(QCPP_ENABLE_TESTS)
    ADD_SUBDIRECTORY(tests)
ENDIF()
ADD_SUBDIRECTORY(progs)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/qc-config.hh.in
               ${CMAKE_BINARY_DIR}/qc-config.hh)

# Library
SET(QCPP_HEADERS
        qcpp.hh
        qc-ds.hh
        qc-io.hh
        qc-processor.hh
        qc-quality.hh
        qc-util.hh
        processors/qc-length.hh
        processors/qc-adaptor.hh
        processors/qc-measure.hh
        processors/qc-qualtrim.hh
        )

SET(LIBQCPP_SRC
	qc-util.cc
        qc-ds.cc
        qc-io.cc
        qc-processor.cc
        qc-quality.cc
        processors/qc-length.cc
        processors/qc-adaptor.cc
        processors/qc-measure.cc
        processors/qc-qualtrim.cc
        )

ADD_LIBRARY(libqcpp SHARED ${LIBQCPP_SRC})
SET_TARGET_PROPERTIES(libqcpp PROPERTIES SONAME_VERSION 0 VERSION 0)
SET_TARGET_PROPERTIES(libqcpp PROPERTIES OUTPUT_NAME qcpp)
TARGET_LINK_LIBRARIES(libqcpp ${DEPENDS_LIBS})

ADD_LIBRARY(libqcppa STATIC ${LIBQCPP_SRC})
SET_TARGET_PROPERTIES(libqcppa PROPERTIES OUTPUT_NAME qcpp)
TARGET_LINK_LIBRARIES(libqcppa ${DEPENDS_LIBS})
TARGET_COMPILE_OPTIONS(libqcppa PUBLIC "-fPIC")

INSTALL(TARGETS libqcpp libqcppa DESTINATION "lib")
INSTALL(FILES ${QCPP_HEADERS} DESTINATION "include")
