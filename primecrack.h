#ifndef PRIMECRACK_H
#define PRIMECRACK_H

typedef struct {
    char *n1;
    char *n2;
} crack_result_t;

int crack(char *numberChar, int base, unsigned int pos, unsigned int count, crack_result_t *result);

#endif
