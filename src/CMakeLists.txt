IF(QCPP_ENABLE_TESTS)
    ADD_SUBDIRECTORY(tests)
ENDIF()
ADD_SUBDIRECTORY(progs)

CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/qc-config.hh.in
               ${CMAKE_BINARY_DIR}/qc-config.hh)

# Library
SET(QCPP_HEADERS
    ${CMAKE_BINARY_DIR}/qc-config.hh
    qcpp.hh
    qc-io.hh
    qc-processor.hh
    qc-length.hh
    qc-adaptor.hh
    qc-measure.hh
    qc-qualtrim.hh
    qc-quality.hh
    qc-util.hh
    )

SET(LIBQCPP_SRC
    qc-util.cc
    qc-io.cc
    qc-processor.cc
    qc-length.cc
    qc-adaptor.cc
    qc-measure.cc
    qc-qualtrim.cc
    qc-quality.cc
    )

if (NOT STATIC_BINARIES)
    add_library(libqcpp SHARED ${LIBQCPP_SRC})
    set_target_properties(libqcpp PROPERTIES SONAME_VERSION 0 VERSION 0)
    set_target_properties(libqcpp PROPERTIES OUTPUT_NAME qcpp)
    target_link_libraries(libqcpp ${QCPPDEPS_LIBS})
    target_include_directories(libqcpp PUBLIC ${QCPPDEPS_INCLUDE_DIRS})
    target_compile_definitions(libqcpp PUBLIC ${QCPPDEPS_DEFINITIONS})

    install(TARGETS libqcpp DESTINATION "lib")
endif()

ADD_LIBRARY(libqcppa STATIC ${LIBQCPP_SRC})
SET_TARGET_PROPERTIES(libqcppa PROPERTIES OUTPUT_NAME qcpp)
target_link_libraries(libqcppa ${QCPPDEPS_LIBS})
target_include_directories(libqcppa PUBLIC ${QCPPDEPS_INCLUDE_DIRS})
target_compile_definitions(libqcppa PUBLIC ${QCPPDEPS_DEFINITIONS})

INSTALL(TARGETS libqcppa DESTINATION "lib")
INSTALL(FILES ${QCPP_HEADERS} DESTINATION "include")
