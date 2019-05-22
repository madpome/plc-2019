#include <stdint.h>
#include "zigzag.c"
//Quantification inverse
//Les types des trucs en entrée et retournés seront à verifier
int32_t ** quant_inv(int32_t *bloc, uint8_t *quant_table){
    //Je suppose mon bloc et mon quant_table sous forme de zigzag
    int32_t * tab = malloc(64*sizeof(int32_t));
    for(int i = 0;i<64;i++){
        tab[i] = bloc[i]*quant_table[i];
    }
    return zigzag_to_array(tab);
}
//A la sortie, on devrait avor un bloc 8x8 contenant les blocs
//de fréquence initiaux
