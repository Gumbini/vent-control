CC = gcc
CXX = g++
CFLAGS = -pedantic -Wall -Wformat=2 -Wshadow -Wconversion -std=gnu17
CXXFLAGS = -pedantic -Wall -Wformat=2 -Wshadow -Wconversion -std=gnu++20

BASEDIR = $(shell pwd)/

OBJPATH = $(BASEDIR)/objects/
CPATH = $(BASEDIR)/src/c/
CXXPATH = $(BASEDIR)/src/cpp/
PJPATH = $(BASEDIR)/pjproject-2.11.1

elf_name = vent-control

cpp_objects = main.cpp.o utility.cpp.o
c_objects = request.c.o sipcall.c.o utility.c.o

elf_objects = $(cpp_objects) $(c_objects)
elf_libs = -lpj -lpjsua

MAKEFLAGS += --jobs=$(shell nproc)
MAKEFLAGS += --output-sync=target

# -----------------------------------------------------------------------

vpath %.c $(CPATH)
vpath %.h $(CPATH)

vpath %.cpp $(CXXPATH)
vpath %.hpp $(CXXPATH)

vpath %.o $(OBJPATH)

.PHONY: all dirs clean debug init pjlib

# -----------------------------------------------------------------------

all: $(elf_name)

clean:
	rm -f $(elf_name)
	cd $(OBJPATH) && rm -f *.o

debug: CFLAGS += -g
debug: CXXFLAGS += -g
debug: all

# -----------------------------------------------------------------------

init: dirs pjlib

dirs:
	mkdir -p $(OBJPATH)

pjlib:
	cd $(PJPATH) \
	&& ./configure --prefix=/usr --enable-shared --disable-video --disable-libwebrtc CFLAGS='-O2' \
	&& $(MAKE) dep \
	&& $(MAKE)
	echo -e "\n\033[31mPlease run \"sudo make install\" in $(PJPATH) to install the library!\n"

# -----------------------------------------------------------------------

$(elf_name): $(elf_objects)
	cd $(OBJPATH) \
	&& $(CXX) -o $(BASEDIR)/$@ $^ $(elf_libs)

#sipcall.c.o: sipcall.c
#	$(CC) $(CFLAGS) -c -I$(CPATH) $^ -o $(OBJPATH)/$@ -lpj -lpjsua -lpjmedia

%.c.o: %.c 
	$(CC) $(CFLAGS) -c -I$(CPATH) $^ -o $(OBJPATH)/$@

%.cpp.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c -I$(CPATH) -I$(CXXPATH) $^ -o $(OBJPATH)/$@

