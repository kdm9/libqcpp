ADD_EXECUTABLE(gbsqc gbsqc.cc)
TARGET_LINK_LIBRARIES(gbsqc libqc++ ${DEPENDS_LIBS})

ADD_EXECUTABLE(threaded threaded.cc)
TARGET_LINK_LIBRARIES(threaded libqc++ ${DEPENDS_LIBS})

ADD_EXECUTABLE(lenfilter lenfilter.cc)
TARGET_LINK_LIBRARIES(lenfilter libqc++ ${DEPENDS_LIBS})
