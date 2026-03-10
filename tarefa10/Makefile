CC     = gcc
CFLAGS = -O3 -fopenmp -march=native -Wall -Wextra

all: pi_critical_comp pi_atomic_comp pi_critical_priv pi_atomic_priv pi_reduction

pi_critical_comp: pi_critical_comp.c
	$(CC) $(CFLAGS) -o $@ $<

pi_atomic_comp: pi_atomic_comp.c
	$(CC) $(CFLAGS) -o $@ $<

pi_critical_priv: pi_critical_priv.c
	$(CC) $(CFLAGS) -o $@ $<

pi_atomic_priv: pi_atomic_priv.c
	$(CC) $(CFLAGS) -o $@ $<

pi_reduction: pi_reduction.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f pi_critical_comp pi_atomic_comp pi_critical_priv pi_atomic_priv pi_reduction
