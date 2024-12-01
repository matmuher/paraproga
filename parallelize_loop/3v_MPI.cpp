#include <mpi.h>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include <stdexcept>

#define $ printf("%d\n", __LINE__);

int main(int argc, char** argv) {
    int world_size;
    int world_rank;

    MPI_Init(&argc, &argv);

    int ISIZE = 10'000;
    int JSIZE = 10'000;

    if (argc == 3) {
        try {
            ISIZE = std::stoi(argv[1]);
            JSIZE = std::stoi(argv[2]); 
        }
        catch (const std::invalid_argument& ex) {
            printf("Cant parse args! %s ISIZE JSIZE\n", argv[0]);
            printf("Example: %s %d %d\n", argv[0], ISIZE, JSIZE);
        }
    }

    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    double* buffer = (double*) calloc(2 * ISIZE, sizeof(double));
    MPI_Buffer_attach(buffer, 2 * ISIZE);

    float start = MPI_Wtime();

/* START*/

    /* Determine my slice */
    int work_size = ((ISIZE + world_size - 1) / world_size);
    int work_start = world_rank * work_size;
    int work_end = (work_start + work_size);
    work_end = (work_end <= ISIZE) ? work_end : ISIZE;
    int local_i_max = work_end - work_start;

    double* local_results_write = (double*) malloc(JSIZE * work_size * sizeof(double));
    double* local_results_read = (double*) malloc(JSIZE * work_size * sizeof(double));

    /* Init data */
    for (int local_i = 0; local_i < local_i_max; local_i++) {
        for (int j = 0; j < JSIZE; j++) {
            local_results_read[JSIZE * local_i + j] = sin(0.01 * (10. * (local_i + work_start) + j));
        }
    }

    /* Process data */
    for (int local_i = 0; local_i < local_i_max; local_i++) {
        for (int j = 2; j < JSIZE; j++) {
            local_results_write[JSIZE * local_i + j] =  local_results_read[JSIZE * local_i + (j-2)] * 2.5; 
        }
    }
/*END*/

/* Gather data */

    double* all_results = (double*) malloc(world_size * JSIZE * work_size * sizeof(double));

    MPI_Gather(
    local_results_write,
    JSIZE * work_size,
    MPI_DOUBLE,
    all_results,
    JSIZE * work_size,
    MPI_DOUBLE,
    0,
    MPI_COMM_WORLD);

    if (world_rank == 0) {

        float end = MPI_Wtime();
        printf("Elapsed time: %f\n", end - start);

        FILE* ff = fopen("3vmpi.txt","w");
        for(int i= 0; i < ISIZE; i++){
                for (int j= 0; j < JSIZE; j++){
                    fprintf(ff,"%f ", j < 2 ? 0. : all_results[i * JSIZE + j]);
                }
            fprintf(ff,"\n");
        }
        fclose(ff);
    }

    MPI_Finalize();

    free(local_results_write);
    free(local_results_read);
    free(all_results);
    return 0;
}
