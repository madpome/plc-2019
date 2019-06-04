#include "../include/color.h"

/* Prend en entrée trois composantes YCbCr et renvoie la composante RGB associée */
RGB ycbcr_to_rgb(float Y, float C_b, float C_r){
    RGB rgb;

    float a = Y + 1.402*(C_r-128);
    a = (a<0)?0:a;
    a = (a>255)?255:a;
    rgb.R =(uint8_t) round(a);

    a = Y - 0.34414*(C_b - 128) - 0.71313*(C_r - 128);
    a = (a<0)?0:a;
    a = (a>255)?255:a;
    rgb.G = (uint8_t) round(a);

    a = Y + 1.772*(C_b - 128);
    a = (a<0)?0:a;
    a = (a>255)?255:a;
    rgb.B =(uint8_t) round(a);

    return rgb;
}


/* Prend en entrée un tableau de flottants et renvoie un tableau de RGB gris
dont seule la composante rouge est initialisée */
RGB **ycbcr_to_gris(float **tab,uint16_t horizontal,uint16_t vertical){
  RGB **rgb = malloc(vertical*sizeof(RGB *));
  for(int i =0;i<vertical;i++){
    rgb[i] = malloc(horizontal*sizeof(RGB));
  }
  for(int i = 0;i<vertical;i++){
    for(int j = 0;j<horizontal;j++){
      rgb[i][j].R = ((uint8_t) (tab[i][j]));
    }
  }
  for (int i = 0; i < 8; i++){
      free(tab[i]);
  }
  free(tab);
  return rgb;
}


/* Copie un tableau (small) dans un plus grand tableau (big) en démarrant par
le coin en haut à de coordonées (x, y) */
void write_big_rgb(RGB **big, int x, int y, RGB ** small, int lignes, int colonnes){
    for(int i =y;i<(y+lignes);i++){
        for(int j =x;j<(x+colonnes);j++){
            big[i][j] = small[i-y][j-x];
        }
    }
}
