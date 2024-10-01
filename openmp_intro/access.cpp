#include <omp.h>
#include <stdio.h>

int main() {
  int res = 0;
  int const thread_num = omp_get_max_threads();

#pragma omp parallel for ordered shared(res)
  for (int thread_idx = 0; thread_idx < thread_num; ++thread_idx)
  {
#pragma omp ordered
    {
      res += thread_idx;
      printf("%d: res= %d\n", thread_idx, res);
    }
  }

  printf("res=%d\n", res);

  return 0;
}
