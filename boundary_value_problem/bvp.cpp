#include <cmath>
#include <cstdio>
#include <cassert>
#include <omp.h>
#include <thread>
#include <vector>

std::vector<std::thread> workers;

struct Coefs {
    double a;
    double b;
    double c;
    double d;
};

Coefs* coefs = NULL;
Coefs* origin_coefs = NULL;

double* y0s = NULL;
double* xs = NULL;
double* gs = NULL;
double* ss = NULL;

double a_param;
double h;
double h2;
double h2_12;

int N;
int chunk;
int reduction_th;

// double f(double y) {
//     return a_param * (y*y*y - y);
// }

double f(double y) {
    return exp(-y);
}

// double df_dy(double y) {
//     return a_param * (3 * y*y - 1);
// }

double df_dy(double y) {
    return -exp(-y);
}

double d2y_dx2(double* y, int n) {

    if (n == 0) {
        return (y[n+1] - 2 * y[n] + y[n+1]) / (h2);
    } else if (n == N-1) {
        return (y[n-1] - 2 * y[n] + y[n-1]) / (h2);
    }

    return (y[n+1] - 2 * y[n] + y[n-1]) / (h2);
}

/*
Linear coef from second order linear diff eq, details:
    https://en.wikipedia.org/wiki/Numerov%27s_method
    https://studfile.net/preview/2465136/page:4/
*/
double g(int n) {
    return -df_dy(y0s[n]);
}


/*
Free coef from second order linear diff eq, details:
    https://en.wikipedia.org/wiki/Numerov%27s_method
    https://studfile.net/preview/2465136/page:4/
*/
double s(int n) {
    return f(y0s[n]) - d2y_dx2(y0s, n);
}

void reduce(int n, int step) {
    assert(step % 2 == 0);
    
    Coefs res = coefs[n];
    int lookup = step/2;

    assert(coefs[n-lookup].b != 0.);
    assert(coefs[n+lookup].b != 0.);

    double prev_factor = -coefs[n].a / coefs[n-lookup].b;
    double next_factor = -coefs[n].c / coefs[n+lookup].b;

    res.a = 0;
    res.a += prev_factor * coefs[n-lookup].a;

    res.b += prev_factor * coefs[n-lookup].c + next_factor * coefs[n+lookup].a;
    
    res.c = 0;
    res.c += next_factor * coefs[n+lookup].c;
    
    res.d += prev_factor * coefs[n-lookup].d + next_factor * coefs[n+lookup].d;
    
    coefs[n] = res;
}

void dump_system(int start = 0, int end = N-1) {
    for (int n = start; n <= end; n+=1) {
        printf("n[%d]: %f %f %f %f\n", n, coefs[n].a, coefs[n].b, coefs[n].c, coefs[n].d);
    }
}

void dump_system2(int start = 0, int end = N-1) {
    printf("n[%d]: %f %f %f %f\n", start, coefs[start].a, coefs[start].b, coefs[start].c, coefs[start].d);

    for (int n = start+1; n <= end-1; n+=1) {
        printf("n[%d]: %f %f %f %f\n", n, origin_coefs[n].a, origin_coefs[n].b, origin_coefs[n].c, origin_coefs[n].d);
    }

    printf("n[%d]: %f %f %f %f\n", end, coefs[end].a, coefs[end].b, coefs[end].c, coefs[end].d);
}

struct ProgonCoef {
    double y;
    double alfa;
    double beta;
};

/*
    Thx for debugging, Dear https://matrixcalc.org/slu.html#solve-using-Gaussian-elimination
*/
void do_progonka(int start, int step) {
    // printf("Progonka [%d, %d] %d\n", start, start + 2*step, start / chunk);
    // dump_system2(start, start+2*step);

    ProgonCoef* progons = (ProgonCoef*) calloc(2*step+1, sizeof(ProgonCoef));
    
    progons[0] = {.alfa = 0. /*-c1/b1*/, .beta = coefs[start].d /*d1/b1*/};

    // forward progonka
    for (int i = 1; i < 2 * step; i++) {
        Coefs cur = origin_coefs[start + i];
        
        double y = cur.b + cur.a * progons[i-1].alfa;

        progons[i].alfa = -(cur.c) / y;
        progons[i].beta = (cur.d - cur.a * progons[i-1].beta) / y;
    
        // printf("%d: alfa=%f, beta=%f\n", i, progons[i].alfa, progons[i].beta);
    }

    // backward progonka
    for (int i = 2*step - 1; i >= 1; i--) {
        coefs[start + i].a = 0.;
        coefs[start + i].b = 1.;
        coefs[start + i].c = 0.;
        coefs[start + i].d = progons[i].alfa * coefs[start + i + 1].d + progons[i].beta;
        
        // printf("p%d: %f <- %f\n", start + i, coefs[start + i].d, coefs[start + i + 1].d);
        // printf("alfa: %f, beta: %f\n", progons[i].alfa, progons[i].beta);
    }

    // printf("prognali:\n");
    // dump_system(start, start+2*step);
}

void inverse_reduction(int start, int step) {
    // printf("InverseReduction[start = %d, step = %d]\n", start, step);

    if (2*step <= reduction_th) {
        int idx = start / chunk;
        workers[idx] = std::thread{do_progonka, start, step};
        return;
    }

    Coefs cur = coefs[start + step];
    double res = (cur.d - cur.a * coefs[start].d - cur.c * coefs[start + step*2].d) / cur.b;
    coefs[start + step] = Coefs{0., 1., 0., res};

    inverse_reduction(start, step/2);
    inverse_reduction(start + step, step/2);
}

double make_time_sample() {
    static double last_sample = omp_get_wtime();
    double current = omp_get_wtime();
    double ret = current - last_sample;
    last_sample = current;
    return ret;
}

void init_coefs(int start, int end) {
    // printf("INIT %d:%d\n", start, end);

    gs[start-1] = g(start-1);
    gs[start] = g(start);

    ss[start-1] = s(start-1);
    ss[start] = s(start);

    for (int n = start; n < end; n++) {
        if (n == 0 || n == N-1) {
            continue;
        }

        gs[n+1] = g(n+1);
        ss[n+1] = s(n+1);

        Coefs cur;

        /* https://en.wikipedia.org/wiki/Numerov%27s_method */

        cur.a = (1 + h2_12 * gs[n-1]);
        cur.b = 2 * (5.* h2_12 * gs[n] - 1);
        cur.c = (1 + h2_12 * gs[n+1]);
        cur.d = h2_12 * (ss[n+1] + 10*ss[n] + ss[n-1]);

        coefs[n] = cur;
        origin_coefs[n] = cur;
    }
}

int main(int argc, const char** argv) {

/* Input params */

    N = (2<<16)+1; // Number of points
    int pass_num = 1;
    int worker_num = 8;

    if (argc >= 2) {
        worker_num = atoi(argv[1]);
        if (worker_num != 1 ||
            worker_num != 2 ||
            worker_num != 4 ||
            worker_num != 8) {
            printf("cmd args err: worker = {1, 2, 4, 8}\n");
        }
    }

    if (argc >= 3) {
        pass_num = atoi(argv[2]);
    }

    chunk = (N-1) / worker_num;
    workers = std::vector<std::thread>(worker_num);

    double const x_st = 0.;
    double const y_st = 1.;

    double const x_end = 1.;
    double y_end = .9;

    double const a_min = 100;
    double const a_max = 1'000'000;

    a_param = a_min;

    if (argc >= 4) {
        y_end = atof(argv[3]);
    }


    h = (x_end - x_st) / (N-1);
    h2 = h*h;
    h2_12 = h2/12.;
    reduction_th = chunk;

    y0s = (double*) calloc(N, sizeof(double));
    xs = (double*) calloc(N, sizeof(double));
    gs = (double*) calloc(N, sizeof(double));
    ss = (double*) calloc(N, sizeof(double));

    printf("N = %d, workers = %d, chunk = %d, h = %f\n", N, worker_num, chunk, h);

/* Init xs */

    xs[0] = x_st;
    xs[N-1] = x_end;

    for (int n = 1; n < N-1; n++) {
        xs[n] = xs[0] + h * n;
    }

/* Init y0s with linear approximation */

    y0s[0] = y_st;
    y0s[N-1] = y_end;

    double const k = (y_end - y_st) / (x_end - x_st);

    for (int n = 1; n < N-1; n++) {
        y0s[n] = y_st + (xs[n] - x_st) * k;
    }

/* Allocate coefs for linear system on nu */

    coefs = (Coefs*) calloc(N, sizeof(Coefs));
    origin_coefs = (Coefs*) calloc(N, sizeof(Coefs));

/* Fill initial coefs for system on nu */

    for (int pass_idx = 0; pass_idx < pass_num; pass_idx++) {

        make_time_sample();

        coefs[0] = {0, 1, 0 , 0};
        origin_coefs[0] = coefs[0]; 
        
        coefs[N-1] = {0, 1, 0, 0};
        origin_coefs[N-1] = coefs[N-1];

        int n;

/* Fill initial coefficients */

        for (int idx = 0; idx < worker_num; idx++) {
            workers[idx] = std::thread{init_coefs, idx * chunk, (idx + 1) * chunk};
        }

        for (int idx = 0; idx < worker_num; idx++) {
            workers[idx].join();
        }

        double fill_coefs_time = make_time_sample();

/* Do the reduction on odd equations */

        int max_step = (N-1)/2;
        for (int step = 2; step <= max_step; step *= 2) {
            
            for (int n = 0; n < N; n+=step) {
                if (n == 0 || n == N-1) {
                    continue;
                }

                reduce(n, step);
            }
        }
        
        double reduction_time = make_time_sample();

/* Do the inverse reduction on reduced equations */

        inverse_reduction(0, max_step);
        
        for (int idx = 0; idx < worker_num; idx++) {
            workers[idx].join();
        }

        double inverse_time = make_time_sample();
        
/* Update y0s */

        for (int n = 0; n < N; n++) {
            y0s[n] = y0s[n] + coefs[n].d;
        }

        double update_time = make_time_sample();
    
        double total_time = update_time +
                            fill_coefs_time +
                            reduction_time +
                            inverse_time;

        printf("\t[TIME] total: %f ms\n"
                "fill coefs: %.1f%%\n"
                "reduction: %.1f%%\n"
                "inverse: %.1f%%\n"
                "update: %.1f%%\n",
                total_time * 1000.,
                100. * fill_coefs_time / total_time,
                100. * reduction_time / total_time,
                100. * inverse_time / total_time,
                100. * update_time / total_time);
    }

    FILE* res_file = fopen("results.txt", "w");
    for (int n = 0; n < N; n++) {
        fprintf(res_file, "%f ", xs[n]);
    }
    
    fprintf(res_file, "\n");

    for (int n = 0; n < N; n++) {
        fprintf(res_file, "%f ", y0s[n]);
    }

    fprintf(res_file, "\n");
    fclose(res_file);

    // system("python plot.py");

    return 0;
}
