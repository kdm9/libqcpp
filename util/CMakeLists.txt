ADD_EXECUTABLE(qcparse qcparse.cc)
TARGET_LINK_LIBRARIES(qcparse libqc++ ${DEPENDS_LIBS})

ADD_EXECUTABLE(qclencount qclencount.cc)
TARGET_LINK_LIBRARIES(qclencount libqc++ ${DEPENDS_LIBS})

ADD_EXECUTABLE(qcpipeline qcpipeline.cc)
TARGET_LINK_LIBRARIES(qcpipeline libqc++ ${DEPENDS_LIBS})

ADD_EXECUTABLE(gbstrim gbstrim.cc)
TARGET_LINK_LIBRARIES(gbstrim libqc++ ${DEPENDS_LIBS})
