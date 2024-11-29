THREAD_NUM := $(shell expr $(JSIZE) - 1)

all: cmp

mpi:
	mpic++ 1a_MPI.cpp
	@mpirun --oversubscribe -np ${THREAD_NUM}  a.out ${ISIZE} ${JSIZE}

ord:
	mpic++ 1a.cpp
	./a.out ${ISIZE} ${JSIZE}

cmp: ord mpi
	diff result.txt result_mpi.txt

