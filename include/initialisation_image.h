#ifndef __INITIALISATION_IMAGE_H__
#define __INITIALISATION_IMAGE_H__

#include <stdint.h>
#include <stdlib.h>
#include "color.h"

extern struct RGB **init_RGB(int lignes, int colonnes);

extern int16_t **init_mcu_quant(int lignes, int colonnes);

extern float **init_mcu_freq(int lignes, int colonnes);

#endif
