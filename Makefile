CXXSRCS := $(wildcard *.cpp)
CXXOBJS := $(CXXSRCS:%.cpp=%.o)

OBJS := $(CXXOBJS)

BIN := egl_test

INCDIRS := /opt/vc/include /opt/vc/include/interface/vcos/pthreads /opt/vc/include/interface/vmcs_host/linux
LIBDIRS := /opt/vc/lib
LIBS := GLESv2 EGL bcm_host vcos vchiq_arm pthread rt m

CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++11 -O2 $(INCDIRS:%=-I%)
LDFLAGS := $(LIBDIRS:%=-L%) $(LIBS:%=-l%)

.PHONY: all check clean

all: $(BIN)

check:
	$(CXX) $(CXXFLAGS) -fsyntax-only $(CXXSRCS)

clean:
	-@rm -vf $(BIN) $(OBJS)

$(BIN): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
