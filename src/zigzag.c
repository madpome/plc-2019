

#include "../include/zigzag.h"
//Transforme un vecteur 1x64 en un bloc 8x8 avec la méthode du zigzag inversé

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

void pixels_to_ppm(RGB **pixels, int largeur, int hauteur,int noir_et_blanc, const char* filename){
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
            if(noir_et_blanc == 0){
                fwrite(&(pixels[i][j].G), sizeof(uint8_t), 1, f);
                fwrite(&(pixels[i][j].B), sizeof(uint8_t), 1, f);
            }
        }
    }
    free(s);
    fclose(f);
}
/*int main(int argc, char ** argv){
    uint8_t n = 25;
    RGB **pixels = malloc(8*sizeof(RGB *));
    for(int i = 0;i<8;i++){
        pixels[i] = malloc(sizeof(RGB));
        pixels[i]->R = n;
        n+=20;
    }
    pixels_to_ppm(pixels, 1, 8, 1, "test");

}*/


/* Transforme un tableau de blocs en tableau 2D de pixels */
struct RGB **bloc2array(struct RGB ****bigarray,uint16_t nb_bloc_h,uint16_t nb_bloc_v){

  /* Taille de l'image sans troncature des bordures */
  uint16_t horizontal_fictif = 8*nb_bloc_h;
  uint16_t vertical_fictif = 8*nb_bloc_v;

  /* Initialisation */
  struct RGB **array = malloc(vertical_fictif*sizeof(struct RGB *));
  for (int i =0; i< vertical_fictif; i++){
    array[i] = malloc(horizontal_fictif*sizeof(struct RGB));
  }

/* On parcourt les blocs (i,j) et (k,l) à l'intérieur des blocs */
  for (int i =0; i<nb_bloc_v; i++){
    for (int j =0; j<nb_bloc_h; j++){
      for (int k =0; k<8; k++){
	       for (int l =0; l<8; l++){
           array[8*i+k][8*j+l] = bigarray[i][j][k][l];
	       }
      }
    }
  }
  return array;
}
