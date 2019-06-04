#include "../include/initialisation_image.h"


/* Initialisae un MCU après quantification */
int16_t **init_mcu_quant(int lignes, int colonnes){
    int16_t **toto = malloc(lignes*sizeof(int16_t*));
    for (int i = 0; i<lignes; i++){
      toto[i] = malloc(colonnes*sizeof(int16_t));
    }
    return toto;
}


/* Initialise un MCU au format RGB */
RGB **init_RGB(int lignes, int colonnes){
    RGB **toto = malloc(lignes*sizeof(RGB *));
    for (int i = 0; i<lignes; i++){
      toto[i] = malloc(colonnes*sizeof(RGB));
    }
    return toto;
}


/* Initialise un MCU après iDCT */
float **init_mcu_freq(int lignes, int colonnes){
    float **toto = malloc(lignes*sizeof(float*));
    for (int i = 0; i<lignes; i++){
      toto[i] = malloc(colonnes*sizeof(float));
    }
    return toto;
}
