#include "../include/mcu.h"
 
RGB **trad_mcu(struct bitstream *stream, struct jpeg_desc *jpeg, int16_t *prec_y, int16_t *prec_cb, int16_t *prec_cr,uint8_t facteur_h, uint8_t facteur_v, uint8_t mcu_lignes, uint8_t mcu_colonnes, uint8_t *quant_table_y, uint8_t *quant_table_c){  

  float ***tab_y = malloc(facteur_v*facteur_h*sizeof(float **));
  RGB ***tab_rgb = malloc(facteur_v*facteur_h*sizeof(RGB **));
  //printf("fac_h = %i, fac_v = %i\n", facteur_h,facteur_v); 
    
  RGB ** mcu_rgb = init_RGB(mcu_lignes, mcu_colonnes);
  for (int i=0; i<facteur_v*facteur_h; i++){
    int16_t *y = trad_composante(stream, jpeg, prec_y, COMP_Y);
    *prec_y = y[0];
    int16_t **quant_y = quant_inv(y, quant_table_y);
    tab_y[i] = idct(quant_y);
  }
  int16_t *cb = trad_composante(stream, jpeg, prec_cb, COMP_Cb);
  *prec_cb = cb[0]; 
  int16_t *cr = trad_composante(stream, jpeg, prec_cr, COMP_Cr);
  *prec_cr = cr[0];
  int16_t **quant_cb = quant_inv(cb, quant_table_c);
  int16_t **quant_cr = quant_inv(cr, quant_table_c);
  float **freq_cb = idct(quant_cb);
  float **freq_cr = idct(quant_cr);
  for (int i = 0; i<facteur_h*facteur_v; i++){
    tab_rgb[i] = tab_ycbcr_to_rgb(tab_y[i],freq_cb,freq_cr);
  }

  int indice_y = 0;
  int top_left_x = 0;
  int top_left_y = 0;
  for (int i=0; i<facteur_v; i++){
    top_left_x = 0;
    for (int j=0; j<facteur_h; j++,indice_y++){
      write_big_rgb(mcu_rgb,top_left_x, top_left_y, tab_rgb[indice_y], 8, 8);
      top_left_x += 8;
    }
    top_left_y += 8;
  }
  return mcu_rgb;
}

RGB **trad_mcu_noir_et_blanc(struct bitstream *stream, struct jpeg_desc *jpeg, int16_t *prec_y, uint8_t *quant_table_y){
  int16_t *y = trad_composante(stream, jpeg, prec_y, COMP_Y);
  *prec_y = y[0];
  int16_t **quant_y = quant_inv(y, quant_table_y);
  float **freq_y = idct(quant_y);
  return ycbcr_to_gris(freq_y,8,8);
}
