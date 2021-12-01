CC = gcc
CXX = g++
CFLAGS = -pedantic -Wall -Wformat=2 -Wshadow -Wconversion -std=gnu17
CXXFLAGS = -pedantic -Wall -Wformat=2 -Wshadow -Wconversion -std=gnu++20 #-Wno-reorder

BASEDIR = $(shell pwd)
OBJPATH = ./objects
CPATH = ./src/c
CXXPATH = ./src/cpp

elf_name = vent-control
elf_objects = main.cpp.o http-query.c.o utility.c.o utility.cpp.o

# -----------------------------------------------------------------------

vpath %.c $(CPATH)
vpath %.h $(CPATH)

vpath %.cpp $(CXXPATH)
vpath %.hpp $(CXXPATH)

vpath %.o $(OBJPATH)

DIRS = $(OBJPATH)

.PHONY: all $(DIRS) clean debug 

all: $(OBJPATH) $(elf_name)

debug: CFLAGS += -g
debug: CXXFLAGS += -g
debug: all



clean:
	rm -f $(elf_name)
	rm -f -d -r $(OBJPATH)

$(DIRS):
	mkdir -p $@


$(elf_name): $(elf_objects)
	cd $(OBJPATH); \
	$(CXX) -o $(BASEDIR)/$@ $^

%.c.o: %.c 
	$(CC) $(CFLAGS) -c -I$(CPATH) $^ -o $(OBJPATH)/$@

%.cpp.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c -I$(CPATH) -I$(CXXPATH) $^ -o $(OBJPATH)/$@

