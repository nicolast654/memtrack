CC := gcc

.PHONY: all
all: memtrack.so main

memtrack.so: memtrack.c hashmap.o utils.o
	${CC} -g -fPIC -shared -o memtrack.so hashmap.o utils.o memtrack.c

hashmap.o: hashmap.c hashmap.h
	${CC} -c -fPIC hashmap.c

utils.o: utils.c utils.h
	${CC} -c -fPIC utils.c

main: main.c
	${CC} -rdynamic -o main main.c

.PHONY: clean
clean:
	rm -f *.so *.o main
