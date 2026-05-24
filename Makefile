CC := gcc

.PHONY: all
all: memtrack.so example

memtrack.so: memtrack.c hashmap.o utils.o
	${CC} -g -fPIC -shared -o memtrack.so hashmap.o utils.o memtrack.c

hashmap.o: hashmap.c hashmap.h
	${CC} -c -fPIC hashmap.c

utils.o: utils.c utils.h
	${CC} -c -fPIC utils.c

example: example.c
	${CC} -rdynamic -o example example.c

.PHONY: install
install: memtrack.so
	mkdir -p /usr/local/lib/memtrack
	cp ./memtrack.so /usr/local/lib/memtrack/memtrack.so
	mkdir -p /usr/local/bin/
	chmod +x  ./memtrack.sh
	cp -p ./memtrack.sh /usr/local/bin/memtrack

.PHONY: clean
clean:
	rm -f *.so *.o example
