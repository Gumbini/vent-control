CC = gcc
CFLAGS = -pedantic -Wall -Wformat=2 -Wshadow -Wconversion -g
CPPFLAGS = -lstdc++ -std=c++20 -Wno-reorder

.PHONY: all clean run

all: vent

clean:
	rm -f vent *.o

run: vent
	./$^

vent: vent.o
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ $^

%.o: %.cpp 
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $^

