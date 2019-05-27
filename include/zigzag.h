#ifndef __ZIGZAG_H__
#define __ZIGZAG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "color.h"

extern int16_t ** zigzag_to_array(int16_t * zigzag);

extern void pixels_to_ppm(RGB **pixels, int largeur, int hauteur, int noir_et_blanc, const char *filename);

extern struct RGB **bloc2array(struct RGB ****bigarray,uint16_t nb_bloc_h,uint16_t nb_bloc_v,uint16_t horizontal,uint16_t vertical);

#endif
