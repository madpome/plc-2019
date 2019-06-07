#include "../include/color.h"

/* Prend en entrée trois composantes YCbCr et renvoie la composante RGB associée */
RGB ycbcr_to_rgb(float Y, float Cb, float Cr){
    RGB rgb;

    float a = Y + 1.402*(Cr-128); //Y
    a = (a<0)?0:a;
    a = (a>255)?255:a;
    rgb.R =(uint8_t) round(a);

    a = Y - 0.34414*(Cb - 128) - 0.71313*(Cr - 128); //Y Cr
    a = (a<0)?0:a;
    a = (a>255)?255:a;
    rgb.G = (uint8_t) round(a);

    a = Y + 1.772*(Cb - 128);
    a = (a<0)?0:a;
    a = (a>255)?255:a;
    rgb.B =(uint8_t) round(a);

    return rgb;
}


/* Prend en entrée un tableau de flottants et renvoie un tableau de RGB gris
dont seule la composante rouge est initialisée */
RGB **ycbcr_to_gris(float **tab, uint16_t horizontal, uint16_t vertical){
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
void write_big_rgb(RGB **big, int x, int y, RGB ** small, int lignes,
                                                          int colonnes){
    for(int i =y;i<(y+lignes);i++){
        for(int j =x;j<(x+colonnes);j++){
            big[i][j] = small[i-y][j-x];
        }
    }
}


/* Prend en entrée un tableau RGB et renvoie l'image correspondante
   au format ppm si couleur ou pgm si niveau de gris */
void pixels_to_ppm(RGB **pixels, int largeur, int hauteur,
                   int noir_et_blanc, const char* filename){
    char *s = malloc(sizeof(char)*(strlen(filename)+5));
    sprintf(s,"%s%s",filename,(noir_et_blanc==1?".pgm":".ppm"));
    FILE *f = fopen(s, "w");
    if(f==NULL){
        fprintf(stderr, "erreur pixels_to_ppm");
        perror("");
        return;
    }

    fprintf(f,(noir_et_blanc==0?"P6\n":"P5\n"));
    fprintf(f,"%d %d\n",largeur, hauteur);
    fprintf(f,"255\n");

    for(int i =0;i<hauteur;i++){
        for(int j = 0;j<largeur;j++){
            fwrite(&(pixels[i][j].R), sizeof(uint8_t), 1, f);
            // Si on est en noir et blanc on ne considère que le R
            if(noir_et_blanc == 0){
                fwrite(&(pixels[i][j].G), sizeof(uint8_t), 1, f);
                fwrite(&(pixels[i][j].B), sizeof(uint8_t), 1, f);
            }
        }
    }
    free(s);
    fclose(f);
}
