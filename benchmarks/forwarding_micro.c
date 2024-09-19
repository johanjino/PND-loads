#include <stdlib.h>
#include <stdint.h>

int main(int argc, char **argv)  {

    int n = atoi(argv[1]);
    volatile int *load = malloc(sizeof(int));
    *load = n*3;
    int *store = malloc(2*n*sizeof(int));
    asm volatile (
        "dsb sy\n\t"
        "isb\n\t"
    );
    double offset = 3.14965 * (n-1);
    double factor = 2.73234 * n;
    *(int *)(store + (uintptr_t)(offset/factor/factor)) = n;
    volatile int y = *load;

    if (y % n == 0)
        return 1;
    return 0;
}
