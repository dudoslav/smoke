#ifndef PRIMECRACK_H
#define PRIMECRACK_H

typedef struct {
    char *n1;
    char *n2;
} crkres_t;

int crack(char *numberChar, unsigned int pos, unsigned int count, crkres_t *crkres);

#endif
