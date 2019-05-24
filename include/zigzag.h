#ifndef __ZIGZAG_H__
#define __ZIGZAG_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "color.h"

extern int32_t ** zigzag_to_array(int32_t * zigzag);

extern void pixels_to_ppm(RGB **pixels, int largeur, int hauteur, int noir_et_blanc, const char *filename);
#endif
