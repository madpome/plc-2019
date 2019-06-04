#ifndef __DECOMP_H__
#define __DECOMP_H__

#include <stdint.h>
#include "huffman.h"
#include "bitstream.h"
#include "jpeg_reader.h"


extern uint16_t get_indice(struct bitstream *stream, uint32_t nb_bits);

extern int16_t trad_DC(struct bitstream *stream, struct jpeg_desc *jpeg,
                       int16_t *prec, enum component comp);

extern struct symbole_AC trad_AC(struct bitstream *stream,
                                 struct jpeg_desc *jpeg, enum component comp);

extern int32_t valeur_magnitude(uint8_t magnitude, uint16_t);

extern int16_t *trad_composante(struct bitstream *stream,
                                struct jpeg_desc *jpeg,
                                int16_t *prec, enum component comp);

struct symbole_AC {
  int EOB;
  int8_t nb_zeros;
  int16_t valeur;
};

#endif
