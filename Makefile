OS := $(shell uname -s)

CXXFLAGS=-O0 -DDEBUG -fPIC -std=c++11
LDFLAGS=-std=c++11



ifeq ($(OS), Linux)
  DYN_LIB_SUFFIX=so
  DYN_LIB_COMMAND=-shared
  PRELOADED=LD_PRELOAD=libnew.$(DYN_LIB_SUFFIX)
  LDFLAGS+=-Wl,-export_dynamic
endif
ifeq ($(OS), Darwin)
  DYN_LIB_SUFFIX=dylib
  DYN_LIB_COMMAND=-dynamiclib
  PRELOADED=DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=libnew.$(DYN_LIB_SUFFIX)
  LDFLAGS+=-Wl,-export_dynamic
endif


all: libnew.(DYN_LIB_SUFFIX)

libnew.(DYN_LIB_SUFFIX): libnew.cpp Makefile
	$(CXX) $(CXXFLAGS) -c -o libnew.o libnew.cpp
	$(CXX) $(LDFLAGS) $(DYN_LIB_COMMAND) -o libnew.$(DYN_LIB_SUFFIX) libnew.o

test-runner: libnew.(DYN_LIB_SUFFIX) Makefile
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o test-runner test.cpp

test: test-runner 
	$(PRELOADED) ./test-runner

test:

clean:
	rm -f *.o
	rm -f *.$(DYN_LIB_SUFFIX)
	rm -f test

.PHONY: test