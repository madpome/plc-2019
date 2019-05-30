#ifndef __COLOR_H__
#define __COLOR_H__

#include <stdint.h>
#include <stdlib.h>
#include <math.h>
typedef struct RGB{
    //Si l'image est en noir et blanc, on met le niveau de gris dans la composante R
    uint8_t R;
    uint8_t G;
    uint8_t B;
}RGB;

extern struct RGB ycbcr_to_rgb(float Y, float C_r, float C_b);
extern struct RGB **tab_ycbcr_to_rgb(float **Y, float **C_r, float **C_b);
extern RGB **ycbcr_to_gris(float **tab,uint16_t horizontal, uint16_t vertical);
extern void write_big_rgb(RGB **big, int x, int y, RGB ** small, int lignes, int colonnes);
#endif
