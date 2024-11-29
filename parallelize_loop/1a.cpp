#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>
#include <mpi.h>

constexpr int ISIZE = 10;
constexpr int JSIZE = 10;

int main(int argc, char **argv)
{
    unsigned ISIZE = 10;
    unsigned JSIZE = 10;

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
    int i, j;
    FILE *ff;
    
    for (i=0; i<ISIZE; i++){
        for (j=0; j<JSIZE; j++){
        a[i][j] = 10*i + j;
        }
    }
    
    float start = MPI_Wtime();
    // start timer
    for (i=1; i<ISIZE; i++){
        for (j = 0; j < JSIZE-1; j++){
            a[i][j] = sin(2*a[i-1][j+1]);
        }
    }
    // stop timer
    float end = MPI_Wtime();
    printf("Elapsed time: %f\n", end - start);

    // ff = fopen("result.txt","w");
    // for(i= 0; i < ISIZE; i++){
    //     for (j= 0; j < JSIZE; j++){
    //         fprintf(ff,"%f ",a[i][j]);
    //     }
    //     fprintf(ff,"\n");
    // }
    // fclose(ff);
}