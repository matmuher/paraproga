#include "timer.hpp"
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  int const N = 1'000'000'0;
  /*
  N = 1'000'000'0

  App name: sum
  sum, OMP_NUM_THREADS=1
      time: 120.981
      res: 16.686031
  sum, OMP_NUM_THREADS=2
      time: 67.0401
      res: 16.696459
  sum, OMP_NUM_THREADS=3
      time: 49.6149
      res: 16.698206
  sum, OMP_NUM_THREADS=4
      time: 39.355
      res: 16.693453
  sum, OMP_NUM_THREADS=5
      time: 39.3751
      res: 16.694305
  sum, OMP_NUM_THREADS=6
      time: 34.3842
      res: 16.699877
  sum, OMP_NUM_THREADS=7
      time: 31.1273
      res: 16.697130
  sum, OMP_NUM_THREADS=8
      time: 26.468
      res: 16.695967
  sum, OMP_NUM_THREADS=9
      time: 24.5313
      res: 16.697802
  sum, OMP_NUM_THREADS=10
      time: 26.5796
      res: 16.695637
  sum, OMP_NUM_THREADS=11
      time: 46.1362
      res: 16.693966
  sum, OMP_NUM_THREADS=12
      time: 57.3671
      res: 16.694893
  */

  int const thread_num = omp_get_max_threads();
  float *res = (float *)calloc(thread_num, sizeof(float));
  int const load = (N + thread_num - 1) / thread_num;
  {
    SimpleTimer::Timer t{"time"};
#pragma omp parallel for
    for (int thread_idx = 0; thread_idx < thread_num; ++thread_idx) {
      float thread_res = 0.;

      int n_start = thread_idx * load;
      int n_end = (thread_idx + 1) * load;
      for (int n = n_end - 1; n >= n_start; --n) {
        if (n < N) {
          thread_res += 1. / (n + 1);
        }
      }
      // printf("%d: from %d to %d = %f\n", thread_idx, n_start, n_end,
      // thread_res);

      res[thread_idx] = thread_res;
    }
  }

  float all_res = 0.;
  for (int thread_idx = thread_num - 1; thread_idx >= 0; --thread_idx) {
    all_res += res[thread_idx];
  }

  printf("res: %f\n", all_res);

  return 0;
}