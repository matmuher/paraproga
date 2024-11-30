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

    int ISIZE = 10;
    int JSIZE = 10;

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
    int work_size = ((JSIZE + world_size - 1) / world_size);
    int work_start = world_rank * work_size;
    int work_end = (work_start + work_size);
    work_end = (work_end <= JSIZE) ? work_end : JSIZE;
    int local_j_max = work_end - work_start;

    /* Local storage: VLA */
    double* local_results = (double*) malloc(ISIZE * work_size * sizeof(double));

    /* Init data */
    /* ALERT: Tail handling */
    for (int local_j = 0; local_j < local_j_max; local_j++) {
        local_results[0 * work_size + local_j] = 10. * 0. + (work_start + local_j);
    }

    if (world_rank != 0) {
        /* Send initial data to left neighbour*/
        MPI_Bsend(local_results, 1, MPI_DOUBLE, world_rank - 1, 0, MPI_COMM_WORLD);
    }

    /* Compute rows */
    for (int row_idx = 1; row_idx < ISIZE; ++row_idx) {

        double dependency;
        MPI_Request request;

        if (world_rank != (world_size - 1)) { /* Receive data from right neighbour */
            MPI_Irecv(&dependency, 1, MPI_DOUBLE, world_rank + 1, 0, MPI_COMM_WORLD, &request);
        }

        for (int local_j = 0; local_j < local_j_max - 1; local_j++) { /* Compute independent cells */
            local_results[row_idx * work_size + local_j] = sin(2 * local_results[(row_idx - 1) * work_size + (local_j + 1)]);
        }

        if (world_rank != 0 && row_idx != (ISIZE - 1)) { /* Send data to left neighbour */
            MPI_Bsend(&(local_results[row_idx * work_size]), 1, MPI_DOUBLE, world_rank - 1, 0, MPI_COMM_WORLD);
        }

        if (world_rank != (world_size - 1)) { /* Compute dependent cell */
            MPI_Wait(&request, MPI_STATUS_IGNORE);
            local_results[row_idx * work_size + (local_j_max - 1)] = sin(2 * dependency);
        } else { /* Rightest executor doesn't do Recv */
            local_results[row_idx * work_size + (local_j_max - 1)] = 10. * row_idx + (work_start + local_j_max - 1);
        }
    }
/*END*/

/* Gather data */

    double* all_results = (double*) malloc(world_size * ISIZE * work_size * sizeof(double));

    MPI_Gather(
    local_results,
    ISIZE * work_size,
    MPI_DOUBLE,
    all_results,
    ISIZE * work_size,
    MPI_DOUBLE,
    0,
    MPI_COMM_WORLD);

    if (world_rank == 0) {

        float end = MPI_Wtime();
        printf("Elapsed time: %f\n", end - start);

        // FILE* ff = fopen("result_mpi.txt","w");
        // for(int i= 0; i < ISIZE; i++){
        //     for (int z = 0; z < world_size; z++) {
        //         for (int j= 0; j < work_size; j++){

        //             if (z * work_size + j >= JSIZE) {
        //                 continue;
        //             }

        //             fprintf(ff,"%f ", all_results[z * work_size * ISIZE + i * work_size + j]);
        //         }
        //     }
        //     fprintf(ff,"\n");
        // }
        // fclose(ff);

    }

    MPI_Finalize();

    free(local_results);
    free(all_results);
    return 0;
}
