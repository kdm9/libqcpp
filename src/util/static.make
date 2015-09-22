CXXFLAGS=-static -static-libstdc++ -std=c++11 -O3 -I src/ -I src/ext/ -I.
LIBS=-L/usr/local/lib -lbz2 -lz -lm -lyaml-cpp -lmpc -lmpfr -lgmp -lpthread
SRCS=$(wildcard src/*.cc src/ext/*.c src/ext/*.cpp)
TESTSRCS=$(wildcard src/tests/*.cc src/test/ext/*.c)
OBJS=$(addsuffix .o,$(basename $(SRCS)))
ARCHIVES=/usr/local/lib/libbz2.a /usr/local/lib/libz.a /usr/local/lib/libyaml-cpp.a /usr/local/lib/libmpc.a /usr/local/lib/libmpfr.a /usr/local/lib/libgmp.a /usr/lib/libm.a /usr/lib/libpthread.a
VERSION=$(shell git describe --always)

all: gbsqc test-qcpp libqcpp.a

qc-config.hh: src/qc-config.hh.in
	sed -e 's/\$${QCPP_VERSION}/$(VERSION)/' $< >$@

%.o: %.cc qc-config.hh
	g++ $(CXXFLAGS) -c -o $@ $<

libqcpp.a: $(OBJS) $(ARCHIVES) qc-config.hh
	ar rcs $@.tmp $(OBJS)
	echo "CREATE $@" > qcpp-script.mri
	list='$^ $@.tmp'; for p in $$list; do \
	    echo "ADDLIB $$p" >> qcpp-script.mri; \
	done
	echo "SAVE" >> qcpp-script.mri
	ar -M < qcpp-script.mri
	ranlib $@
	rm -f $@.tmp qcpp-script.mri

%: ./src/progs/%.cc $(SRCS) qc-config.hh
	g++ $(CXXFLAGS) -o $@ $< $(SRCS) $(LIBS)

test-qcpp: $(TESTSRCS) qc-config.hh
	g++ $(CXXFLAGS) -I src/tests/ -I src/tests/ext/ -o $@ $(TESTSRCS) $(SRCS) $(LIBS)

libdist: libqcpp.a $(SRCS) qc-config.hh
	rm -rf libqcpp_$(VERSION)
	mkdir libqcpp_$(VERSION)
	mkdir libqcpp_$(VERSION)/lib
	mkdir libqcpp_$(VERSION)/include
	cp src/*.hh libqcpp_$(VERSION)/include
	cp qc-config.hh libqcpp_$(VERSION)/include
	cp libqcpp.a libqcpp_$(VERSION)/lib
	tar cvJf libqcpp_$(VERSION).tar.xz libqcpp_$(VERSION)

bindist: all
	rm -rf libqcpp_$(VERSION)
	mkdir libqcpp_$(VERSION)
	mkdir libqcpp_$(VERSION)/bin
	cp gbsqc test-qcpp libqcpp_$(VERSION)/bin
	tar cvJf qcpp-bin_$(VERSION).tar.xz libqcpp_$(VERSION)

.Phony: dist
dist: bindist libdist

clean:
	rm -rf *.mri *.a *.o gbsqc test-qcpp libqcpp_* qc-config.hh
