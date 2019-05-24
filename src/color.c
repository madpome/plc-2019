#include "../include/color.h" 

struct RGB ycbcr_to_rgb(float Y, float C_r, float C_b){
    RGB rgb;
    float a = Y + 1.402*(C_r-128);
    a = (a<0)?0:a;
    a = (a>255)?255:a;
    rgb.R = (uint8_t) a;
    a = Y - 0.34414*(C_b - 128) - 0.71313*(C_r - 128);
    a = (a<0)?0:a;
    a = (a>255)?255:a;
    rgb.G = (uint8_t) a;
    a = Y + 1.772*(C_b - 128);
    a = (a<0)?0:a;
    a = (a>255)?255:a;
    rgb.B = (uint8_t ) a;
}

RGB **ycbcr_to_gris(float **tab){
  RGB **rgb = malloc(8*sizeof(RGB *));
  for(int i =0;i<8;i++){
    rgb[i] = malloc(8*sizeof(RGB));
  }
  for(int i = 0;i<8;i++){
    for(int j = 0;j<8;j++){
      rgb[i][j].R = (uint8_t) tab[i][j];
    }
  }
  return rgb;
}
