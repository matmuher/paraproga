#include <mpi.h>
#include <cstdio>
#include <cmath>
#include <cassert>
#include <string>
#include <vector>
#include <stdexcept>

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

    double results[ISIZE-1] = {}; /* from row=1 to row=ISIZE-1 */

/* START*/
    float start = MPI_Wtime();

    for (int row_idx = 1; row_idx < ISIZE; ++row_idx) {
        /* Determine my slice */

        /* Wait for dependency */
        double dependency = 10 * (row_idx-1) + (world_rank+1);
        
        if (world_rank != (JSIZE-2) && row_idx != 1) {
            MPI_Recv(&dependency, 1, MPI_DOUBLE, world_rank+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        /* Do my slice */
        double result = sin(2*dependency);
        results[row_idx-1] = result;

        /* Send to next executor */
        if (world_rank != 0 && row_idx != (ISIZE-1)) {
            MPI_Send(&result, 1, MPI_DOUBLE, world_rank-1, 0, MPI_COMM_WORLD);
            /* TODO: do the buffering */
        }
    }

    if (world_rank == 1)
        printf("%d %f\n", world_rank, results[0]);

/*END*/

/* Gather data */
    if (world_rank != 0) { 
        MPI_Send(&results, ISIZE-1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }
    
    if (world_rank == 0) {
        
        double all_results[ISIZE][JSIZE];

        for (int executor = 1; executor < world_size; ++executor) {
            double executor_result[ISIZE-1];
            MPI_Status status;
            MPI_Recv(&executor_result, ISIZE-1, MPI_DOUBLE, executor, 0, MPI_COMM_WORLD, &status);

            for (int row_idx = 0; row_idx < ISIZE-1; ++row_idx) {
                all_results[row_idx+1][executor] = executor_result[row_idx];
            }
            all_results[0][executor] = executor;
        }

        for (int row_idx = 0; row_idx < ISIZE-1; ++row_idx) {
            all_results[row_idx+1][0] = results[row_idx];
            all_results[row_idx+1][JSIZE-1] = 10 * (row_idx+1) + (JSIZE-1);
        }
        all_results[0][JSIZE-1] = JSIZE-1;
        all_results[0][0] = 0;

        // FILE* ff = fopen("result_mpi.txt","w");
        // for(int i= 0; i < ISIZE; i++){
        //     for (int j= 0; j < JSIZE; j++){
        //         fprintf(ff,"%f ", all_results[i][j]);
        //     }
        //     fprintf(ff,"\n");
        // }
        // fclose(ff);
        float end = MPI_Wtime();
        printf("Elapsed time: %f\n", end - start);

    }

    MPI_Finalize();
    return 0;
}
