#ifndef __IDCT_H__
#define __IDCT_H__

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

extern float ** naive_idct(int16_t **fequence, float ** table_cos);
extern float ** cos_table();
extern float ** idct(int16_t **frequence);
extern float* idct_colonne(float **frequence, int col);
extern float* idct_ligne(float* frequence, int16_t *freq_true, int free_it);
#endif
