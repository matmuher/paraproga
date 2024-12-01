THREAD_NUM ?= 19

all: cmp

1ampi:
	mpic++ 1a_MPI.cpp
	mpirun --oversubscribe -np $(THREAD_NUM)  a.out ${ISIZE} ${JSIZE}

1aord:
	mpic++ 1a.cpp
	./a.out ${ISIZE} ${JSIZE}

2bopenmp:
	g++ -fopenmp 2b_OPENMP.cpp
	./a.out

3vmpi:
	mpic++ 3v_MPI.cpp
	mpirun --oversubscribe -np $(THREAD_NUM)  a.out ${ISIZE} ${JSIZE}

3vord:
	mpic++ 3v.cpp
	./a.out ${ISIZE} ${JSIZE}
