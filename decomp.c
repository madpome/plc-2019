#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/huffman.h"
#include "include/plan.h"
#include "include/bitstream.h"


int8_t read_high(int8_t octet){
	/* mask = 0b11110000 */ 
	int8_t mask = -16;
	/* décalage de 4 pour bien récupérer la valeur */ 
	return (octet & mask)>>4;
}

int8_t read_low(int8_t octet){
	/* mask = 0b00001111 */ 
	int8_t mask = 15; 
	return (octet & mask);
}




int32_t valeur_magnitude(uint8_t magnitude, uint16_t indice){
	if (indice >= 1<<(magnitude-1))
	{
		return indice;
	}
	else
	{
		printf("%i\n",1<<magnitude);
		return -(1<<magnitude) +1 +indice;
	}
	  
}



uint16_t get_indice(struct bitstream *stream, uint32_t nb_bits){
	uint32_t dest;
	read_bitstream(stream, nb_bits, &dest, false);
	return dest;
}


int32_t trad_DC(struct bitstream *stream, struct jpeg_desc *jpeg){
	struct huff_table *huffman = get_huffman_table(jpeg, DC, 0);
	int8_t magnitude = next_huffman_value(huffman, stream);
	uint16_t indice = get_indice(stream, magnitude);
	return valeur_magnitude(magnitude, indice);
}


int8_t trad_AC(struct bitstream *stream, struct jpeg_desc *jpeg){
	struct huff_table *huffman = get_huffman_table(jpeg, AC, 0);
	int8_t octet = next_huffman_value(huffman, stream);
	int8_t nb_zeros = read_high(octet);
	int8_t magnitude = read_low(octet);
	uint16_t indice = get_indice(stream, magnitude);
	return (nb_zeros, valeur_magnitude(magnitude, indice));
}



int main(int argc, char *argv[]){
	printf("%i\n",atoi(argv[1]));
	printf("%i\n",valeur_magnitude(atoi(argv[1]),atoi(argv[2])));
	printf("%i\n", read_low(50));
}

