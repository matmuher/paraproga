#include <omp.h>
#include <stdio.h>

int main() {
  int res = 0;
  int current_thread = 0;

#pragma omp parallel shared(res, current_thread)
  {
    const int thread_idx = omp_get_thread_num();
    while (current_thread != thread_idx) {
      ;
    }

    res += thread_idx;
    printf("%d: res= %d\n", thread_idx, res);
    current_thread++;
  }

  #pragma omp barrier
  printf("res=%d\n", res);

  return 0;
}
