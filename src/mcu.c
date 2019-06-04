#include "../include/mcu.h"

/* Traduit entièrement un MCU en prenant en entrée le bitstream et en
   renvoyant un tableau de RGB */
RGB **trad_mcu(struct bitstream *stream, struct jpeg_desc *jpeg,
               int16_t *prec_y, int16_t *prec_cb, int16_t *prec_cr,
               uint8_t facteur_h, uint8_t facteur_v,
               uint8_t mcu_lignes, uint8_t mcu_colonnes,
               uint8_t *quant_table_y, uint8_t *quant_table_c){

    // Initialisation d'une liste qui contient tous les blocs de la composante Y
    float ***liste_y = malloc(facteur_v*facteur_h*sizeof(float **));

    // Initialisation du MCU
    RGB ** mcu_rgb = init_RGB(mcu_lignes, mcu_colonnes);

    /* On fait passer toutes les compoantes Y dans la chaine et on les stock
       dans une liste */
    for (int i = 0; i < facteur_v*facteur_h; i++){
        int16_t *y = trad_composante(stream, jpeg, prec_y, COMP_Y);
        // On fait pointer le prec vers le DC qui vient d'être traduit
        *prec_y = y[0];
        int16_t **quant_y = quant_inv(y, quant_table_y);
        liste_y[i] = idct(quant_y);
    }

    float **tab_y = y_to_tab(liste_y,facteur_h,facteur_v);

    // On traduit les composantes Cb et Cr
    int16_t *cb = trad_composante(stream, jpeg, prec_cb, COMP_Cb);
    *prec_cb = cb[0];
    int16_t *cr = trad_composante(stream, jpeg, prec_cr, COMP_Cr);
    *prec_cr = cr[0];

    int16_t **quant_cb = quant_inv(cb, quant_table_c);
    int16_t **quant_cr = quant_inv(cr, quant_table_c);

    float **freq_cb = idct(quant_cb);
    float **freq_cr = idct(quant_cr);

    // On reconstitue le MCU avec les composantes qu'il convient d'utiliser
    for (int i = 0; i<mcu_lignes; i++){
        for (int j = 0; j<mcu_colonnes;j++){
            mcu_rgb[i][j] = ycbcr_to_rgb(tab_y[i][j],
                                         freq_cb[i/facteur_v][j/facteur_h],
                                         freq_cr[i/facteur_v][j/facteur_h]);
        }
    }
  return mcu_rgb;
}


/* Traduit entièrement un MCU en prenant en entrée le bistream et en renvoyant
   un tableau de nuances de gris */
RGB **trad_mcu_noir_et_blanc(struct bitstream *stream, struct jpeg_desc *jpeg,
                             int16_t *prec_y, uint8_t *quant_table_y){

    int16_t *y = trad_composante(stream, jpeg, prec_y, COMP_Y);
    *prec_y = y[0];
    int16_t **quant_y = quant_inv(y, quant_table_y);
    float **freq_y = idct(quant_y);
    return ycbcr_to_gris(freq_y, 8, 8);
}


/* Prend en entrée une liste de blocs de composante Y et renvoie un tableau où
   les blocs sont bien positionnés selon le sens de lecture
   (de gauche à droite et de haut en bas) */
float **y_to_tab(float ***liste_y, uint8_t facteur_h, uint8_t facteur_v){
    float **res = malloc(8*facteur_v*sizeof(float *));
    for (int i=0; i<8*facteur_v;i++){
        res[i] = malloc(8*facteur_h*sizeof(float));
    }

    int indice_y = 0;
    for (int i=0; i<facteur_v; i++){
        for (int j=0; j<facteur_h; j++,indice_y++){
            for (int k=0; k<8;k++){
	              for (int l=0; l<8; l++){
	                  res[8*i+k][8*j+l]=liste_y[indice_y][k][l];
	              }
            }
        }
    }
    return res;
}
