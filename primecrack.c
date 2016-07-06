#include "primecrack.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <getopt.h>
#include <gmp.h>

typedef struct {
    mpz_t n1;
    mpz_t n2;
} divres_t; //Division result struct

static int sqrtInvertStepFind(mpz_t number, divres_t *result, unsigned int pos, unsigned int count) {
    //unsigned long int steps[] = {2, 2, 4, 2};

    int exitCode = 0;

    mpz_t pivotEnd, pivotStart, sqrtNum, interval, iter;
    mpz_init(pivotEnd);
    mpz_init(pivotStart);
    mpz_init(sqrtNum);
    mpz_init(interval);

    mpz_sqrt(sqrtNum, number);
    mpz_cdiv_q_ui(interval, sqrtNum, count);
    if (pos == 1) {
        mpz_set_ui(pivotStart, 2);
    } else {
        mpz_mul_ui(pivotStart, interval, pos - 1);
    }

    if (pos == count) {
        mpz_set(pivotEnd, sqrtNum);
    } else {
        mpz_mul_ui(pivotEnd, interval, pos);
    }

    printf("Looking for factors in: <");
    mpz_out_str(stdout, 10, pivotStart);
    printf(", ");
    mpz_out_str(stdout, 10, pivotEnd);
    printf(">\n");

    mpz_init(iter);
    mpz_set(iter, pivotStart);
    while (mpz_cmp(iter, pivotEnd) <= 0) {

        if (mpz_divisible_p(number, iter)) {
            mpz_set(result->n1, iter);
            mpz_cdiv_q(result->n2, number, iter);
            exitCode = 1;
            break;
        }

        mpz_add_ui(iter, iter, 1);

    }
    mpz_clear(iter);

    mpz_clear(pivotEnd);
    mpz_clear(pivotStart);
    mpz_clear(sqrtNum);
    mpz_clear(interval);

    printf("Done cracking with code: %d\n", exitCode);
    return exitCode;
}

int crack(char *numberChar, int base, unsigned int pos, unsigned int count, crack_result_t *result) {
    mpz_t number;
    mpz_init(number);

    divres_t divres;
    mpz_init(divres.n1);
    mpz_init(divres.n2);

    int exitCode = 0;

    if (!mpz_set_str(number, numberChar, base)) {
        exitCode = sqrtInvertStepFind(number, &divres, pos, count);
    } else {
        fprintf(stderr, "ERROR: failed to convert string into number. String: %s\n", numberChar);
    }

    if (exitCode) {
        result->n1 = malloc(mpz_sizeinbase (divres.n1, 10) + 2);
        result->n2 = malloc(mpz_sizeinbase (divres.n2, 10) + 2);
        mpz_get_str(result->n1, 10, divres.n1);
        mpz_get_str(result->n2, 10, divres.n2);
    }

    mpz_clear(number);
    mpz_clear(divres.n1);
    mpz_clear(divres.n2);

    return exitCode;
}
