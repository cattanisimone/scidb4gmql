ifeq ($(SCIDB),) 
  X := $(shell which scidb 2>/dev/null)
  ifneq ($(X),)
    X := $(shell dirname ${X})
    SCIDB := $(shell dirname ${X})
  endif
  $(info SciDB installed at $(SCIDB))
endif

# A development environment will have SCIDB_VER defined, and SCIDB
# will not be in the same place... but the 3rd party directory *will*
# be, so build it using SCIDB_VER .
ifeq ($(SCIDB_VER),)
  SCIDB_3RDPARTY = $(SCIDB)
else
  SCIDB_3RDPARTY = /opt/scidb/$(SCIDB_VER)
endif

# A better way to set the 3rdparty prefix path that does not assume an
# absolute path...
ifeq ($(SCIDB_THIRDPARTY_PREFIX),)
  SCIDB_THIRDPARTY_PREFIX := $(SCIDB_3RDPARTY)
endif

INSTALL_DIR = $(SCIDB)/lib/scidb/plugins

# Include the OPTIMIZED flags for non-debug use
OPTIMIZED=-O2 -DNDEBUG
DEBUG=-g -ggdb3
CFLAGS = -pedantic -W -Wextra -Wall -Wno-variadic-macros -Wno-strict-aliasing \
         -Wno-long-long -Wno-unused-parameter -fPIC -D_STDC_FORMAT_MACROS \
         -Wno-system-headers -isystem  $(OPTIMIZED) -D_STDC_LIMIT_MACROS -std=c99
CXXFLAGS = -pedantic -W -Wextra -Wall -Wno-variadic-macros -Wno-strict-aliasing \
         -Wno-long-long -Wno-unused-parameter -fPIC $(OPTIMIZED) 
INC = -I. -DPROJECT_ROOT="\"$(SCIDB)\"" -I"$(SCIDB_THIRDPARTY_PREFIX)/3rdparty/boost/include/" \
      -I"$(SCIDB)/include" -I./extern

LIBS = -shared -Wl,-soname,libcu.so -ldl -L. \
       -L"$(SCIDB_THIRDPARTY_PREFIX)/3rdparty/boost/lib" -L"$(SCIDB)/lib" \
       -Wl,-rpath,$(SCIDB)/lib:$(RPATH)

SRCS = Logicalcu.cpp \
       Physicalcu.cpp

# Compiler settings for SciDB version >= 15.7
ifneq ("$(wildcard /usr/bin/g++-4.9)","")
  CC := "/usr/bin/gcc-4.9"
  CXX := "/usr/bin/g++-4.9"
  CXXFLAGS+=-std=c++11 -DCPP11
else
  ifneq ("$(wildcard /opt/rh/devtoolset-3/root/usr/bin/gcc)","")
    CC := "/opt/rh/devtoolset-3/root/usr/bin/gcc"
    CXX := "/opt/rh/devtoolset-3/root/usr/bin/g++"
    CXXFLAGS+=-std=c++11 -DCPP11
  endif
endif

all: libcu.so

clean:
	rm -rf *.so *.o

libcu.so: $(SRCS)
	@if test ! -d "$(SCIDB)"; then echo  "Error. Try:\n\nmake SCIDB=<PATH TO SCIDB INSTALL PATH>"; exit 1; fi
	$(CXX) $(CXXFLAGS) $(INC) -o Logicalcu.o -c Logicalcu.cpp
	$(CXX) $(CXXFLAGS) $(INC) -o Physicalcu.o -c Physicalcu.cpp
	$(CXX) $(CXXFLAGS) $(INC) -o libcu.so plugin.cpp Logicalcu.o Physicalcu.o $(LIBS)
	@echo "Now copy libcu.so to $(INSTALL_DIR) on all your SciDB nodes, and restart SciDB."

test:
	echo write me XXX
