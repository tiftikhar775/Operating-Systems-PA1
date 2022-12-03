#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

typedef struct 
{
    size_t bits;
    size_t n;
} ThreadReturn;

// function using pointers to get significant bits
ThreadReturn *getMostSignificantBits(double d, size_t n) 
{
    ThreadReturn *result = (ThreadReturn *)calloc(1, sizeof(ThreadReturn));
    result->n = n;
    for (size_t i = 1; i <= n; ++i) 
    {
        double p = 1.0 / (1 << i);
        if (d >= p) {
            d -= p;
            result->bits |= 1;
        }
        result->bits <<= 1;
    }
    return result;
}

typedef struct 
{
    size_t i;
    double const *probs;
} ThreadArg;

// function to encode input file
void *encode(void *arg) 
{
    ThreadArg *targ = (ThreadArg *) arg;
    double F = targ->probs[targ->i] / 2;
    for (size_t j = 0; j < targ->i; ++j) 
    {
        F += targ->probs[j];
    }
    return (void *)getMostSignificantBits(F, ceil(-log2(targ->probs[targ->i])) + 1.0);
}

int main(void) 
{
    // read symbols from input
    char symbols[1024];
    scanf("%[^\n]s", symbols);
    size_t const n = (strlen(symbols) >> 1) + 1;
    double *probs = (double *)calloc(n, sizeof(double));
    pthread_t *tids = (pthread_t *)calloc(n, sizeof(pthread_t));
    ThreadArg *targs = (ThreadArg *)calloc(n, sizeof(ThreadArg));

    for (long i = 0; i < n; ++i) 
    {
        scanf("%lf", probs + i);
        targs[i] = (ThreadArg){i, probs};
        pthread_create(tids + i, NULL, encode, (void *) (targs + i));
    }
    // display output
    puts("SHANNON-FANO-ELIAS Codes:");
    for (size_t i = 0; i < n; ++i) 
    {
        ThreadReturn *code;
        pthread_join(tids[i], (void **) &code);
        printf("Symbol %c, Code: ", symbols[i * 2]);
        for (size_t i = code->n; i > 0; --i) 
        {
            printf("%ld", (code->bits >> i) & 1);
        }
        free(code);
        puts("");
    }

    // free used memory
    free(targs);
    free(tids);
    free(probs);
}