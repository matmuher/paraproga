#include <cmath>
#include <cstdio>
#include <cassert>

struct Coefs {
    float a;
    float b;
    float c;
    float d;
};

Coefs* coefs = NULL;
Coefs* coefs_ = NULL;

float* y0s = NULL;
float* xs = NULL;
float* nus = NULL;

float a_param;
float h;
float h2;

int N;

float f(float y) {
    return a_param * (y*y*y - y);
}

float df_dy(float y) {
    return a_param * (3 * y*y - 1);
}

float d2y_dx2(float* y, int n) {
    // assert(n != 1 && n != N-1);
    return (y[n+1] - 2 * y[n] + y[n-1]) / (h*h);
}

/* Sample in y0(n) */
float g(int n) {
    return -df_dy(y0s[n]);
}

/* Sample in y0(n) */
float s(int n) {
    return f(y0s[n]) - d2y_dx2(y0s, n);
}

void reduce(int n, int step) {
    Coefs res = coefs[n];
    // assert(step % 2 == 0);
    int lookup = step/2;

    float prev_factor = -coefs[n].a / coefs[n-lookup].b;
    float next_factor = -coefs[n].c / coefs[n+lookup].b;

    res.a = 0;
    res.a += prev_factor * coefs[n-lookup].a;

    res.b += prev_factor * coefs[n-lookup].c + next_factor * coefs[n+lookup].a;
    
    res.c = 0;
    res.c += next_factor * coefs[n+lookup].c;
    
    res.d += prev_factor * coefs[n-lookup].d + next_factor * coefs[n+lookup].d;
    
    coefs[n] = res;
}

void dump_system(const char* msg = NULL) {
    if (msg != NULL) {
        printf("%s\n\n", msg);
    }
    
    for (int n = 0; n < N; n+=1) {
        printf("n[%d]: %.2f %.2f %.2f %.2f\n", n, coefs[n].a, coefs[n].b, coefs[n].c, coefs[n].d);
    }
}

void inverse_reduction(int start, int step) {
    // printf("Pass[start = %d, step = %d]\n", start, step);

    Coefs cur = coefs[start + step];
    float res = (cur.d - cur.a * coefs[start].d - cur.c * coefs[start + step*2 + 1].d) / cur.b;
    coefs[start + step] = Coefs{0., 1., 0., res};

    if (step > 1) {
        inverse_reduction(start, step/2);
        inverse_reduction(start + step, step/2);
    }
}

int main() {

    float const x_st = 0.;
    // float const y_st = sqrt(2);
    float const y_st = 0.;

    float const x_end = 10.;
    // float const y_end = sqrt(2);
    float const y_end = 100.;

    float const a_min = 1;
    float const a_max = 1'000'000;

    float const h_factor = 10.; // 1'000;

    a_param = a_min;

    h = (1 / sqrt(a_param)) / h_factor;
    h2 = h*h;

    N = (x_end - x_st) / (h) + 1;
    N = 17;
    h = (x_end - x_st) / (N-1);

    printf("h = %.2f, N = %d\n", h, N);

    y0s = (float*) calloc(N, sizeof(float));
    xs = (float*) calloc(N, sizeof(float));
    nus = (float*) calloc(N, sizeof(float));

    /* Init xs */

    xs[0] = x_st;
    xs[N-1] = x_end;

    printf("%.1f ", xs[0]);
    for (int n = 1; n < N-1; n++) {
        xs[n] = xs[0] + h * n;
        printf("%.1f ", xs[n]);
    }
    printf("%.1f\n", xs[N-1]);

    /* Init y0s with linear approximation */

    y0s[0] = y_st;
    y0s[N-1] = y_end;

    float const k = (y_end - y_st) / (x_end - x_st);

    printf("%.1f ", y0s[0]);
    for (int n = 1; n < N-1; n++) {
        y0s[n] = y_st + (xs[n] - x_st) * k;
        printf("%.1f ", y0s[n]);
    }
    printf("%.1f\n", y0s[N-1]);

    /* Allocate coefs for linear system on nu */

    coefs = (Coefs*) calloc(N, sizeof(Coefs));
    coefs_ = (Coefs*) calloc(N, sizeof(Coefs));

    /* Fill initial coefs for system on nu */

    int pass_num = 2;
    for (int pass_idx = 0; pass_idx < pass_num; pass_idx++) {

        printf("!!! PASS IDX !!!: %d\n", pass_idx);

        coefs[0] = {1, 0, 0 , 0};
        coefs[N-1] = {0, 0, 1, 0};
        
        for (int n = 1; n < N-1; n++) {
            Coefs cur;
            
            /* https://en.wikipedia.org/wiki/Numerov%27s_method */
            cur.a = (1 + h2*g(n-1)/12.);
            cur.b = 2 * (5./12.*h2*g(n) - 1);
            cur.c = (1 + h2*g(n+1)/12.);
            cur.d = h2/12.*(s(n+1) + 10*s(n) + s(n-1));

            cur.a = 1.;
            cur.b = -2.;
            cur.c = 1.;
            cur.d = (2 - d2y_dx2(y0s, n))* h2;

            coefs[n] = cur;
        }

        /* Do the reduction on odd equations */

        int max_step = (N-1)/2;
        for (int step = 2; step <= max_step; step *= 2) {
            // printf("Pass[step = %d]\n", step);
            
            for (int n = 0; n < N; n+=step) {
                if (n == 0 || n == N-1) {
                    continue;
                }

                // printf("reduce: n = %d\n", n);
                reduce(n, step);
            }

            // dump_system();
            // printf("\n");
        }
        
        /* Do the inverse reduction on reduced equations */

        inverse_reduction(0, max_step);
        
        // printf("Inversed system\n\n");
        // dump_system();

        /* Update y0s */
        printf("Updated y0s:\n");
        for (int n = 0; n < N; n++) {
            y0s[n] = y0s[n] + coefs[n].d;
            printf("%.1f ", y0s[n]);
        }
        printf("\n");

        printf("nus:\n");
        for (int n = 0; n < N; n++) {
            y0s[n] = y0s[n] + coefs[n].d;
            printf("%.1f ", coefs[n].d);
        }
        printf("\n");
    }

    return 0;
}
