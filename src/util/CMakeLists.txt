ADD_EXECUTABLE(qcparse qcparse.cc)
TARGET_LINK_LIBRARIES(qcparse libqc++ ${DEPENDS_LIBS})

ADD_EXECUTABLE(gbsqc gbsqc.cc)
TARGET_LINK_LIBRARIES(gbstrim libqc++ ${DEPENDS_LIBS})
