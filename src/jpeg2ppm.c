#include "../include/bitstream.h"
#include "../include/huffman.h"
#include "../include/idct.h"
#include "../include/jpeg_reader.h"
#include "../include/decomp.h"
#include "../include/zigzag.h"
#include "../include/quant_inv.h"




int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "Mauvais nombre d'arguments.\n");
        return 1;
    }

    struct jpeg_desc *jpeg = read_jpeg(argv[1]);
    if(jpeg == NULL){
        fprintf(stderr, "Problème lors de l'ouverture du fichier %s\n", argv[1]);
        return 1;
    }
    /*TODO
        get bitstream du jpeg pour travailler dessus
        Faut il extraire en avance toutes les tables ou on le fait que quand on
        en a besoin ?
        decompression(jpeg)
    */
    close_jpeg(jpeg);
    return 0;
}
