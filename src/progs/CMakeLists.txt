ADD_EXECUTABLE(gbsqc gbsqc.cc)
TARGET_LINK_LIBRARIES(gbsqc libqc++ ${DEPENDS_LIBS})

ADD_EXECUTABLE(trimit trimit.cc)
TARGET_LINK_LIBRARIES(trimit libqc++ ${DEPENDS_LIBS})

ADD_EXECUTABLE(gbsqc_serial gbsqc_serial.cc)
TARGET_LINK_LIBRARIES(gbsqc_serial libqc++ ${DEPENDS_LIBS})

ADD_EXECUTABLE(lenfilter lenfilter.cc)
TARGET_LINK_LIBRARIES(lenfilter libqc++ ${DEPENDS_LIBS})
