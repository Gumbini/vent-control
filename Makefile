CC = g++
CFLAGS = -pedantic -Wall -Wformat=2 -Wshadow -Wconversion -g
CPPFLAGS = -std=c++20 -Wno-reorder #-lstdc++

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

