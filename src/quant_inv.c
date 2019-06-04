#include "../include/quant_inv.h"


/* Prend en entrée un bloc (sous forme de zigzag) et une table de quantification
   et renvoie le bloc déquantifié sous d'un tableau 8x8 */
int16_t ** quant_inv(int16_t *bloc, uint8_t *quant_table){
    int16_t * tab = malloc(64*sizeof(int16_t));
    for(int i = 0;i<64;i++){
        tab[i] = bloc[i]*quant_table[i];
    }
    return zigzag_to_array(tab);
}
