#include <stdlib.h>
#include <stdint.h>

int main(int argc, char **argv)  {

    int n = atoi(argv[1]);
    volatile int **forward = malloc(sizeof(int *));
    //volatile int **pnd = malloc(sizeof(int *));
    volatile int *pnd;
    int *m = malloc(n*sizeof(int));
    *forward = (int *)0x423423;
    asm volatile (
        "dsb sy\n\t"
        "isb\n\t"
    );
    double offset = 3.14965 * (n-1);
    double factor = 2.73234 * n;
    *forward = (int *)(m + (uintptr_t)(offset/factor/factor));
    /* *forward = m; */
    //*pnd = *forward;
    pnd = *forward;

    volatile int y = *pnd;

    if (y % n == 0)
        return 1;
    return 0;
}
