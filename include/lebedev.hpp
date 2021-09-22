#pragma once

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int ld0006_(double *x, double *y, double *z, double *w, int *n);
int ld0014_(double *x, double *y, double *z, double *w, int *n);
int ld0026_(double *x, double *y, double *z, double *w, int *n);
int ld0038_(double *x, double *y, double *z, double *w, int *n);
int ld0050_(double *x, double *y, double *z, double *w, int *n);
int ld0074_(double *x, double *y, double *z, double *w, int *n);
int ld0086_(double *x, double *y, double *z, double *w, int *n);
int ld0110_(double *x, double *y, double *z, double *w, int *n);
int ld0146_(double *x, double *y, double *z, double *w, int *n);
int ld0170_(double *x, double *y, double *z, double *w, int *n);
int ld0194_(double *x, double *y, double *z, double *w, int *n);
int ld0230_(double *x, double *y, double *z, double *w, int *n);
int ld0266_(double *x, double *y, double *z, double *w, int *n);
int ld0350_(double *x, double *y, double *z, double *w, int *n);
int ld0434_(double *x, double *y, double *z, double *w, int *n);
int ld0590_(double *x, double *y, double *z, double *w, int *n);
int ld0770_(double *x, double *y, double *z, double *w, int *n);
int ld0974_(double *x, double *y, double *z, double *w, int *n);
int ld1202_(double *x, double *y, double *z, double *w, int *n);
int ld1454_(double *x, double *y, double *z, double *w, int *n);
int ld1730_(double *x, double *y, double *z, double *w, int *n);
int ld2030_(double *x, double *y, double *z, double *w, int *n);
int ld2354_(double *x, double *y, double *z, double *w, int *n);
int ld2702_(double *x, double *y, double *z, double *w, int *n);
int ld3074_(double *x, double *y, double *z, double *w, int *n);
int ld3470_(double *x, double *y, double *z, double *w, int *n);
int ld3890_(double *x, double *y, double *z, double *w, int *n);
int ld4334_(double *x, double *y, double *z, double *w, int *n);
int ld4802_(double *x, double *y, double *z, double *w, int *n);
int ld5294_(double *x, double *y, double *z, double *w, int *n);
int ld5810_(double *x, double *y, double *z, double *w, int *n);

static int LDNS[] = {6, 14, 26, 38, 50, 74, 86, 110, 146, 170, 194, 230, 266, 302, 350, 434,
                     590, 770, 974, 1202, 1454, 1730, 2030, 2354, 2702, 3074, 3470, 3890,
                     4334, 4802, 5294, 5810};

// return weight and grid with given number of points
// the number must be a prior choosen from LDNS
// 6, 14, 26, 38, 50, 74, 86, 110, 146, 170, 194, 230, 266, 302, 350, 434,
// 590, 770, 974, 1202, 1454, 1730, 2030, 2354, 2702, 3074, 3470, 3890,
// 4334, 4802, 5294, 5810
int ld(double *x, double *y, double *z, double *w, int *n);