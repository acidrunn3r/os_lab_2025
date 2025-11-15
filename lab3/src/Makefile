CC=gcc
CFLAGS=-I.
TARGETS=sequential_min_max parallel_min_max process_launcher
OBJECTS=utils.o find_min_max.o

all: $(TARGETS)

sequential_min_max: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) sequential_min_max.c $(CFLAGS)

parallel_min_max: $(OBJECTS)
	$(CC) -o $@ $(OBJECTS) parallel_min_max.c $(CFLAGS)

process_launcher: 
	$(CC) -o $@ process_launcher.c $(CFLAGS)

utils.o: utils.c utils.h
	$(CC) -o $@ -c utils.c $(CFLAGS)

find_min_max.o: find_min_max.c find_min_max.h utils.h
	$(CC) -o $@ -c find_min_max.c $(CFLAGS)

clean:
	rm -f $(OBJECTS) $(TARGETS)

.PHONY: all clean