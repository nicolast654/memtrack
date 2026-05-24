# memtrack
`memtrack` is a simple memory leak tracker written in C to deepen my comprehension of dynamic linking, symbol tables, and shared objects.
It tracks heap allocations (only the ones done using `malloc`, `realloc`, and `calloc`) and reports at the end how many element were allocated and not freed.

## Build
```bash
sudo make install
```
Will build and install the `memtrack` bash script into `/usr/local/bin` and `memtrack.so` to `/usr/local/lib/memtrack/memtrack.so` for it to be used from anywhere on your Linux device.

You can also run:
```bash
make
```
to build without installing.

## Run
*Important:* in order to see function names in the backtrace you need to compile the C program using `-rdynamic`. Otherwise you'd only be able to see how many bytes were leaked across how many allocations.

Example run:

```bash
memtrack ./example
```
Expected output:
```
LEAK DETECTED:
  malloc
  function3
  function2
  function1
  main
4 bytes leaked across 1 memory leaks!
```

> If `rdynamic` is not included, it will just display `????` instead of function names

## How it Works
Uses `LD_PRELOAD` to intercept allocation calls via a shared library and track each allocation and free.

## Limitations
This simple leak tracker has a lot of limitations:

1. Reports all unfreed allocations at exit, not just unreachable memory. Some programs intentionally skip freeing at exit.
2. Statically linked programs cannot be tracked since there are no malloc interpositions.
3. Only tracks allocation made using `malloc`, `calloc`, or `realloc`, allocations using `mmap`, `brk`, `sbrk`, and other will not be reported.
4. Assumes less than 65536 (current size of custom hashmap) leaks per program.
5. Assumes that depth of function call that has leak is less than 16.

