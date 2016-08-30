OS := $(shell uname -s)

CXXFLAGS=-O0 -DDEBUG -fPIC -std=c++11
LDFLAGS=-std=c++11



ifeq ($(OS), Linux)
  DYN_LIB_SUFFIX=so
  LIBDL_LINK=-ldl
  DYN_LIB_COMMAND=-shared -rdynamic $(LIBDL_LINK)
  PRELOADED=LD_PRELOAD=./libnew.$(DYN_LIB_SUFFIX)
endif
ifeq ($(OS), Darwin)
  DYN_LIB_SUFFIX=dylib
  LIBDL_LINK=
  DYN_LIB_COMMAND=-dynamiclib -Wl,-export_dynamic
  PRELOADED=DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=./libnew.$(DYN_LIB_SUFFIX)
endif


all: libnew.(DYN_LIB_SUFFIX)

libnew.(DYN_LIB_SUFFIX): libnew.cpp Makefile
	$(CXX) $(CXXFLAGS) -c -o libnew.o libnew.cpp
	$(CXX) -o libnew.$(DYN_LIB_SUFFIX) libnew.o $(LDFLAGS) $(DYN_LIB_COMMAND)

test-runner: libnew.(DYN_LIB_SUFFIX) Makefile
	$(CXX) $(CXXFLAGS) -o test-runner test.cpp $(LDFLAGS) $(LIBDL_LINK)

test: test-runner 
	$(PRELOADED) ./test-runner

test:

clean:
	rm -f *.o
	rm -f *.$(DYN_LIB_SUFFIX)
	rm -f test

.PHONY: test