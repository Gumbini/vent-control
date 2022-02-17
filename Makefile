CC = gcc
CXX = g++
CFLAGS = -pedantic -Wall -Wformat=2 -Wshadow -Wconversion -std=gnu17 -O2
CXXFLAGS = -pedantic -Wall -Wformat=2 -Wshadow -Wconversion -std=gnu++20 -O2

basedir = $(shell pwd)/

build_path = $(basedir)/build/
include_path = $(basedir)/include/
source_path = $(basedir)/src/

elf_name = vent-control

modules = main.cpp.o request.c.o utility.c.o
#modules = main.cpp.o request.c.o sipcall.c.o utility.c.o
libraries = -lpj -lpjsua
pj_path = $(basedir)/pjproject-2.11.1

# -----------------------------------------------------------------------

MAKEFLAGS += --jobs=$(shell nproc)
MAKEFLAGS += --output-sync=target

vpath %.c   $(source_path)
vpath %.cpp $(source_path)

vpath %.h   $(include_path)
vpath %.hpp $(include_path)

vpath %.o   $(build_path)

.PHONY: all dirs clean debug init pjlib

# -----------------------------------------------------------------------

all: $(elf_name)

clean:
	rm -f $(elf_name)
	rm -f $(build_path)/*.o

debug: CFLAGS += -g
debug: CXXFLAGS += -g
debug: all

# -----------------------------------------------------------------------

init: dirs pjlib

dirs:
	mkdir -p $(build_path)

pjlib:
	cd $(pj_path) \
	&& ./configure --prefix=/usr --enable-shared --disable-video --disable-libwebrtc CFLAGS='-O2' \
	&& $(MAKE) dep \
	&& $(MAKE)
	echo -e "\n\033[31mPlease run \"sudo make install\" in $(pj_path) to install the library!\n"

# -----------------------------------------------------------------------

main.cpp.o:  request.h stringMacros.h
request.c.o: utility.h
sipcall.c.o: utility.h
#utility.c.o: ---

# -----------------------------------------------------------------------

$(elf_name): $(modules)
	cd $(build_path) \
	&& $(CXX) -o $(basedir)/$@ $^ $(libraries)

%.c.o: %.c
	$(CC) $(CFLAGS) -c -I$(include_path) $< -o $(build_path)/$@

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -I$(include_path) $< -o $(build_path)/$@

