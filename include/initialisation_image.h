#include <stdint.h>
#include <stdlib.h>

extern int16_t ***init_image(uint16_t nb_bloc_v, uint16_t nb_bloc_h);

extern int16_t ****init_image_quant(uint16_t nb_bloc_v, uint16_t nb_bloc_h);

extern float ****init_image_freq(uint16_t nb_bloc_v, uint16_t nb_bloc_h);

extern struct RGB ****init_image_RGB(uint16_t nb_bloc_v, uint16_t nb_bloc_h);
