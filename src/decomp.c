#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/huffman.h"
#include "../include/decomp.h"
#include "../include/bitstream.h"
#include "../include/initialisation_image.h"

/* lit les 4 bits de poids fort d'un octet et renvoie l'entier correspondant */
uint8_t read_high(uint8_t octet){
		// décalage de 4 à droite et mask 0xF = 1111
		return (octet>>4) & 0xF;
}


/* lit les 4 bits de poids faible d'un octet et renvoie l'entier correspondant*/
uint8_t read_low(uint8_t octet){
		// mask = 1111
		return (octet & 0xF);
}


/* A partir de la classe de magnitude et d'un indice dans cette classe,
	 renvoie la valeur associée */
int32_t valeur_magnitude(uint8_t magnitude, uint16_t indice){
		if (indice >= 1<<(magnitude-1)){
				return indice;
		}else{
				return -(1<<magnitude) +1 +indice;
	  }
}


/* Le stream doit être positionné juste après un symbole huffman,
	 lit le bon nombre de bits et renvoie l'indice associée */
uint16_t get_indice(struct bitstream *stream, uint32_t nb_bits){
		uint32_t dest;
		read_bitstream(stream, nb_bits, &dest, true);
		return (uint16_t)dest;
}


/* Le stream doit être positionné au début d'un symbole DC,
	 lit le symbole et renvoie la valeur associée */
int16_t trad_DC(struct bitstream *stream, struct jpeg_desc *jpeg, int16_t *prec,
								enum component comp){

    // On récupère l'indice de la bonne table de huffman
  uint8_t indice_DC = get_frame_component_huffman_index(jpeg, (uint8_t) comp, DC);

		struct huff_table *huffman = get_huffman_table(jpeg, DC, indice_DC);
		int8_t magnitude = next_huffman_value(huffman, stream);
		uint16_t indice = get_indice(stream, magnitude);

		// *prec pointe sur le dernier coefficient DC d'une même composante
		return valeur_magnitude(magnitude, indice) + *prec;
}


/* Le stream doit être positionné au début d'un symbole AC,
	 lit le symbole et renvoie le nombre de 0 puis la valeur associée, ou bien EOB */
struct symbole_AC trad_AC(struct bitstream *stream, struct jpeg_desc *jpeg,
												  enum component comp){

    // On récupère l'indice de la bonne table de huffman
  	uint8_t indice_AC = get_frame_component_huffman_index(jpeg, (uint8_t)comp,
																													AC);

  	struct huff_table *huffman = get_huffman_table(jpeg, AC, indice_AC);
  	int8_t octet = next_huffman_value(huffman, stream);
  	uint8_t nb_zeros = read_high(octet);
  	uint8_t magnitude = read_low(octet);

		// Symbole spéciaux
  	if (magnitude == 0){
    		if (nb_zeros == 15){
						// RLE
	    			struct symbole_AC retour = {0,nb_zeros,0};
	    			return retour;
      	}
      	if (nb_zeros == 0){
						// EOB
	    			struct symbole_AC retour = {1,0,0};
	    			return retour;
      	}
  	}
  	uint16_t indice = get_indice(stream, magnitude);
  	struct symbole_AC retour = {0,nb_zeros, valeur_magnitude(magnitude, indice)};
  	return retour;
}

/* Lit un bloc d'une composante de couleur et renvoie un tableau de taille 64
	 contenant la valeur en fréquence de chaque pixel du bloc */
int16_t *trad_composante(struct bitstream *stream, struct jpeg_desc *jpeg,
												 int16_t *prec, enum component comp){

	  // Création du bloc en initialisant toutes les pixels à 0
    int16_t *bloc = calloc(64, sizeof(int16_t));
  	bloc[0] = trad_DC(stream, jpeg, prec, comp);

  	int i = 1;

		// Lecture des 63 AC
  	while (i < 64){
				struct symbole_AC symbole = trad_AC(stream, jpeg, comp);

				if (symbole.EOB == 1){
						// Si on lit un EOB on quitte la boucle et on passe au bloc suivant
						break;
				}
				// Décalage du nombre de zéros lus et écriture de la valeur
    		i+= symbole.nb_zeros;
    		bloc[i] = symbole.valeur;
    		i++;
  	}
  	return bloc;
}
