#include <cstdio>
#include <cmath>
#include <string>
#include <vector>
#include <stdexcept>
#include <omp.h>

int main(int argc, char **argv)
{
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
            b[i][j] = a[i][j];
        }
    }

    double start = omp_get_wtime();
    #pragma omp parallel for collapse(2) private(i, j)
    for (i=0; i<ISIZE-1; i++){
        for (j = 6; j < JSIZE; j++){
            a[i][j] = sin(0.2*b[i+1][j-6]);
        }
    }
    double end = omp_get_wtime();

    printf("Elapsed time: %f\n", end - start);

    // ff = fopen("2b_result_openmp.txt","w");
    // for(i= 0; i < ISIZE; i++){
    //     for (j= 0; j < JSIZE; j++){
    //         fprintf(ff,"%f ",a[i][j]);
    //     }
    //     fprintf(ff,"\n");
    // }
    // fclose(ff);
}
