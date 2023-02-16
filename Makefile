SHELL := /bin/bash

N=32
P=4
H=$(shell echo $$(( $(P) / 2 )))

CC=gcc
MPI_CC=mpicc
FLAGS=-lm -Wall

all: serial pthreads omp mpi hybrid

run_all: all run_serial run_pthreads run_omp run_mpi run_hybrid

serial: serial.c
	$(CC) serial.c -lpthread $(FLAGS) -o serial

run_serial: serial
	time ./serial $N

pthreads: pthreads.c
	$(CC) pthreads.c -lpthread $(FLAGS) -o pthreads

run_pthreads: pthreads
	time ./pthreads $N $P

omp: omp.c
	$(CC) omp.c -fopenmp $(FLAGS) -o omp

run_omp: omp
	time ./omp $N $P

mpi: mpi.c
	$(MPI_CC) mpi.c $(FLAGS) -o mpi

run_mpi: mpi
	time mpirun -np $P mpi $N

hybrid: hybrid.c
	$(MPI_CC) hybrid.c $(FLAGS) -fopenmp -o hybrid

run_hybrid: hybrid
	time mpirun -np $H hybrid $N

clean: 
	rm -f serial pthreads omp mpi hybrid
