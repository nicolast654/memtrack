CC := gcc

.PHONY: all
all: memtrack.so main

memtrack.so: memtrack.c hashmap.o
	${CC} -fPIC -shared -o memtrack.so hashmap.o memtrack.c

hashmap.o: hashmap.c hashmap.h
	${CC} -c -fPIC hashmap.c

main: main.c
	${CC} -o main main.c

.PHONY: clean
clean:
	rm -f *.so *.o main
