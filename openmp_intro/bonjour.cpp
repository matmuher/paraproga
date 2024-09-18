#include <stdio.h>
#include <omp.h>

int main()
{
    printf("Cores num: %d\n", omp_get_num_procs());

    #pragma omp parallel
    printf("Bonjour, je suis thread #%d!\n", omp_get_thread_num());

    return 0;
}