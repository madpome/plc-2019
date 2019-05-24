#include "../include/quant_inv.h"
//Quantification inverse
//Les types des trucs en entrée et retournés seront à verifier
int16_t ** quant_inv(int16_t *bloc, uint8_t *quant_table){
    //Je suppose mon bloc et mon quant_table sous forme de zigzag
    int16_t * tab = malloc(64*sizeof(int16_t));
    for(int i = 0;i<64;i++){
        tab[i] = bloc[i]*quant_table[i];
    }
    return zigzag_to_array(tab);
}
//A la sortie, on devrait avor un bloc 8x8 contenant les blocs
//de fréquence initiaux
 
 
