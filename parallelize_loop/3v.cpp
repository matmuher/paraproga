#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>
#include <mpi.h>

int main(int argc, char **argv) {

    unsigned ISIZE = 10'000;
    unsigned JSIZE = 10'000;

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

    std::vector<std::vector<double>> a{ISIZE, std::vector<double>(JSIZE, 0)};
    std::vector<std::vector<double>> b{ISIZE, std::vector<double>(JSIZE, 0)};

    int i, j;
    FILE *ff;
    for (i=0; i<ISIZE; i++){
        for (j=0; j<JSIZE; j++){
            a[i][j] = 10*i +j;
            b[i][j] = 0;
        }
    }

    float start = MPI_Wtime();
    //начало измерения времени

    for (i=0; i<ISIZE; i++){
        for (j = 0; j < JSIZE; j++){
            a[i][j] = sin(0.01*a[i][j]);
        }
    }

    for (i=0; i<ISIZE; i++){
        for (j = 2; j < JSIZE; j++){
            b[i][j] = a[i][j-2]*2.5;
        }
    }

    //окончание измерения времени
    float end = MPI_Wtime();
    printf("Elapsed time: %f\n", end - start);

    ff = fopen("3v.txt","w");
    for(i=0; i < ISIZE; i++){
        for (j=0; j < JSIZE; j++){
            fprintf(ff,"%f ", b[i][j]);
        }
        fprintf(ff,"\n");
    }
    fclose(ff);
}