#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "include/plan.h"
#include "include/bitstream.h"

int valeur_magnitude(uint8_t magnitude, uint16_t indice){
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



uint16_t get_ind(struct bitstream *stream, uint32_t nb_bits){
	uint32_t dest;
	read_bitstream(stream, nb_bits, &dest, false);
	return dest;
}

int32_t trad_DC(struct bitstream *stream, int32_t prec, struct jpeg_desc *jpeg){
	
}




int main(int argc, char *argv[]){
	printf("%i\n",atoi(argv[1]));
	printf("%i\n",valeur_magnitude(atoi(argv[1]),atoi(argv[2])));
}

