#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/huffman.h"
#include "../include/decomp.h"
#include "../include/bitstream.h"

/* lit les 4 bits de poids fort d'un octet et renvoie l'entier correspondant */
int8_t read_high(int8_t octet){
	/* mask = 0b11110000 */
	int8_t mask = -16;
	/* décalage de 4 pour bien récupérer la valeur */
	return (octet & mask)>>4;
}

/* lit les 4 bits de poids failbe d'un octet et renvoie l'entier correspondant*/
int8_t read_low(int8_t octet){
	/* mask = 0b00001111 */
	int8_t mask = 15;
	return (octet & mask);
}



/* à partir de la classe de magnitude et d'un indice dans cette classe, renvoie la valeur associée */
int32_t valeur_magnitude(uint8_t magnitude, uint16_t indice){
	if (indice >= 1<<(magnitude-1)){
		return indice;
	}else{
		return -(1<<magnitude) +1 +indice;
	}

}


/* le stream doit être positionné juste après un symbole huffman, lit le bon nombre de bits et renvoie la valeur de l'indice associée */
uint16_t get_indice(struct bitstream *stream, uint32_t nb_bits){
	uint32_t dest;
	read_bitstream(stream, nb_bits, &dest, true);
	return (uint16_t)dest;
}


/* le stream doit être positionné au début d'un symbole DC, lit le symbole et renvoie la valeur associée */
int16_t trad_DC(struct bitstream *stream, struct jpeg_desc *jpeg){
	struct huff_table *huffman = get_huffman_table(jpeg, DC, 0);
	int8_t magnitude = next_huffman_value(huffman, stream);
	uint16_t indice = get_indice(stream, magnitude);
	return valeur_magnitude(magnitude, indice);
}

/* le stream doit être positionné au début d'un symbole AC, lit le symbole et renvoie le nombre de 0 le précédent, la valeur associée, ou bien EOB */
struct symbole_AC trad_AC(struct bitstream *stream, struct jpeg_desc *jpeg){
  struct huff_table *huffman = get_huffman_table(jpeg, AC, 0);
  int8_t octet = next_huffman_value(huffman, stream);
  int8_t nb_zeros = read_high(octet);
  int8_t magnitude = read_low(octet);
  if (magnitude == 0){
      if (nb_zeros == 15){
	     struct symbole_AC retour = {0,nb_zeros,0};
	     return retour;
      }
      if (nb_zeros == 0){
	//	printf("end of block\n");
	     struct symbole_AC retour = {1,0,0};
	     return retour;
      }
  }
  uint16_t indice = get_indice(stream, magnitude);
  struct symbole_AC retour = {0,nb_zeros, valeur_magnitude(magnitude, indice)};
  return retour;
}

/*lit 1 bloc et renvoie un tableau de taille 64 contenant la valeur en fréquence de chaque pixel du bloc */
//TODO AFREE
int16_t *trad_bloc(struct bitstream *stream, struct jpeg_desc *jpeg){
  int16_t *bloc = malloc(64*sizeof(int16_t));
  bloc[0] = trad_DC(stream,jpeg);

  int i = 1;
  struct symbole_AC symbole = trad_AC(stream,jpeg);
  while (!symbole.EOB){
      for (int j =0; j<symbole.nb_zeros; j++, i++){
	  	bloc[i] = 0;
		//	printf("i %d\n",i);
      }
      // printf("i = %d\n",i);
      bloc[i] = symbole.valeur;
      symbole = trad_AC(stream,jpeg);
      // printf("%x\n", symbole.valeur);
      i++;
  }

  for (int j = i; j <= 63-i; j++){
      bloc[j] = 0;
  }
  return bloc;
}

int16_t **trad_image(struct bitstream *stream, struct jpeg_desc *jpeg, uint16_t nb_bloc_h, uint16_t nb_bloc_v){
  int16_t **image = malloc(nb_bloc_h*sizeof(int16_t *));
  for (int i=0; i<nb_bloc_h; i++)
    {
      image[i] = malloc(nb_bloc_v*sizeof(int16_t));
    }
  
  for (int j = 0; j< nb_bloc_v;j++)
    {
      for (int i = 0; i< nb_bloc_h; i++)
	{
	  image[i][j] = *trad_bloc(stream,jpeg);
	}
    }
  
  return image;
}

/*int main(int argc, char *argv[]){
	if(argc!=3){
		return -1;
	}
	printf("%i\n",atoi(argv[1]));
	printf("%i\n",valeur_magnitude(atoi(argv[1]),atoi(argv[2])));
	printf("%i\n", read_low(50));
}*/
