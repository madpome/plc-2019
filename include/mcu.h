#ifndef __MCU_H__
#define __MCU_H__

#include "../include/bitstream.h"
#include "../include/color.h"
#include "../include/decomp.h"
#include "initialisation_image.h"
#include "quant_inv.h"
#include "idct.h"

typedef struct prec{
    int16_t *y;
    int16_t *cb;
    int16_t *cr;
}prec;

extern RGB **trad_mcu(struct bitstream *stream, struct jpeg_desc *jpeg,
                      struct prec,
                      uint8_t facteur_h, uint8_t facteur_v,
                      uint8_t mcu_lignes, uint8_t mcu_colonnes,
                      uint8_t *quant_table_y, uint8_t *quant_table_cb,
                      uint8_t *quant_table_cr);

extern RGB **trad_mcu_noir_et_blanc(struct bitstream *stream,
                                    struct jpeg_desc *jpeg,
                                    int16_t *prec_y, uint8_t *quant_table_y);

float **y_to_tab(float ***liste_y, uint8_t facteur_h, uint8_t facteur_v);

#endif
