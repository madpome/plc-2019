#include "../include/zigzag.h"


/* Transforme un vecteur 1x64 en un bloc 8x8 avec la méthode du zigzag inversé */
int16_t ** zigzag_to_array(int16_t * zigzag){
    int tabi[64] = {0,1,5,6,14,15,27,28,2,4,7,13,16,26,29,42,3,8,12,17,25,30,41,\
        43,9,11,18,24,31,40,44,53,10,19,23,32,39,45,52,54,20,22,33,38,46,51,55,\
        60,21,34,37,47,50,56,59,61,35,36,48,49,57,58,62,63};
    int16_t **tab = malloc(8*sizeof(int16_t *));
    for(int i=0;i<8;i++){
        tab[i] = malloc(8*sizeof(int32_t));
    }
    for(int i =0;i<8;i++){
        for(int j = 0;j<8;j++){
            tab[i][j] = zigzag[tabi[8*i+j]];
        }
    }
    free(zigzag);
    return tab;
}
