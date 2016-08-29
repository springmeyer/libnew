OS := $(shell uname -s)

ifeq ($(OS), Linux)
  DYN_LIB_SUFFIX=so
  DYN_LIB_COMMAND=-shared
endif
ifeq ($(OS), Darwin)
  DYN_LIB_SUFFIX=dylib
  DYN_LIB_COMMAND=-dynamiclib
endif


all: libnew.dylib

libnew.dylib: libnew.cpp Makefile
	$(CXX) -O0 -DDEBUG -c -o libnew.o libnew.cpp -std=c++11
	$(CXX) -Wl,-export_dynamic $(DYN_LIB_COMMAND) -o libnew.$(DYN_LIB_SUFFIX) libnew.o -std=c++11

test: libnew.dylib Makefile
	$(CXX) -o test test.cpp
	DYLD_FORCE_FLAT_NAMESPACE=1 DYLD_INSERT_LIBRARIES=libnew.$(DYN_LIB_SUFFIX) ./test

clean:
	rm -f *.o
	rm -f *.$(DYN_LIB_SUFFIX)
	rm -f test

.PHONY: test