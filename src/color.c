#include "../include/color.h"

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
  return rgb;
}

void write_big_rgb(RGB **big, int x, int y, RGB ** small, int lignes, int colonnes){
    for(int i =y;i<(y+lignes);i++){
        for(int j =x;j<(x+colonnes);j++){
            big[i][j] = small[i-y][j-x];
        }
    }
}

struct RGB **tab_ycbcr_to_rgb(float **Y, float **C_r, float **C_b){
  RGB **tab = malloc(8*sizeof(RGB *));
  for (int i=0; i<8; i++){
    tab[i] = malloc(8*sizeof(RGB));
  }
  for (int i=0; i<8; i++){
    for (int j=0; j<8; j++){
      tab[i][j] = ycbcr_to_rgb(Y[i][j],C_r[i][j],C_b[i][j]);
    }
  }
  return tab;
}
