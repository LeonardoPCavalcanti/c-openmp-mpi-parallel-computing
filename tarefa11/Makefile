CC=gcc
CFLAGS=-O3 -fopenmp -march=native -Wall -Wextra
LDLIBS=-lm

all: ns_serial ns_omp_simple ns_omp_static ns_omp_dynamic ns_omp_guided

ns_serial: ns_serial.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

ns_omp_simple: ns_omp_simple.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

ns_omp_static: ns_omp_static.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

ns_omp_dynamic: ns_omp_dynamic.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

ns_omp_guided: ns_omp_guided.c
	$(CC) $(CFLAGS) -o $@ $< $(LDLIBS)

clean:
	rm -f ns_serial ns_omp_simple ns_omp_static ns_omp_dynamic ns_omp_guided
