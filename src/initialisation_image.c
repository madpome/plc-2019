#include "../include/initialisation_image.h"

/* Initialisation de l'image qui comporte les blocs */
int16_t ***init_image(uint16_t nb_bloc_v, uint16_t nb_bloc_h){
  int16_t ***image = malloc(nb_bloc_v*sizeof(int16_t **));
  for (int i = 0; i<nb_bloc_v; i++){
    image[i] = malloc(nb_bloc_h*sizeof(int16_t *));
  }
  return image;
}
int16_t **init_mcu_quant(int lignes, int colonnes){
    int16_t **toto = malloc(lignes*sizeof(int16_t*));
    for (int i = 0; i<lignes; i++){
      toto[i] = malloc(colonnes*sizeof(int16_t));
    }
    return toto;
}
RGB **init_RGB(int lignes, int colonnes){
    RGB **toto = malloc(lignes*sizeof(RGB *));
    for (int i = 0; i<lignes; i++){
      toto[i] = malloc(colonnes*sizeof(RGB));
    }
    return toto;
}
/* Initialisation image après quantification */
int16_t ****init_image_quant(uint16_t nb_bloc_v, uint16_t nb_bloc_h){
  int16_t ****image = malloc(nb_bloc_v*sizeof(int16_t ***));
  for (int i = 0; i<nb_bloc_v; i++){
    image[i] = malloc(nb_bloc_h*sizeof(int16_t **));
  }
  return image;
}

/* Initialisation image après iDCT*/
float ****init_image_freq(uint16_t nb_bloc_v, uint16_t nb_bloc_h){
  float ****image = malloc(nb_bloc_v*sizeof(float ***));
  for (int i = 0; i<nb_bloc_v; i++){
    image[i] = malloc(nb_bloc_h*sizeof(float **));
  }
  return image;
}
float **init_mcu_freq(int lignes, int colonnes){
    float **toto = malloc(lignes*sizeof(float*));
    for (int i = 0; i<lignes; i++){
      toto[i] = malloc(colonnes*sizeof(float));
    }
    return toto;
}

/* Initialisation image en RGB */
struct RGB ****init_image_RGB(uint16_t nb_bloc_v, uint16_t nb_bloc_h){
  struct RGB ****image = malloc(nb_bloc_v*sizeof(struct RGB ***));
  for (int i = 0; i<nb_bloc_v; i++){
    image[i] = malloc(nb_bloc_h*sizeof(struct RGB **));
  }
  return image;
}
