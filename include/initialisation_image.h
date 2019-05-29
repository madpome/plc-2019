#include <stdint.h>
#include <stdlib.h>
#include "color.h"
extern int16_t ***init_image(uint16_t nb_bloc_v, uint16_t nb_bloc_h);

extern int16_t ****init_image_quant(uint16_t nb_bloc_v, uint16_t nb_bloc_h);

extern float ****init_image_freq(uint16_t nb_bloc_v, uint16_t nb_bloc_h);

extern struct RGB ****init_image_RGB(uint16_t nb_bloc_v, uint16_t nb_bloc_h);

extern struct RGB **init_RGB(int lignes, int colonnes);
extern int16_t **init_mcu_quant(int lignes, int colonnes);
extern float **init_mcu_freq(int lignes, int colonnes);
