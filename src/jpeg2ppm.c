#include "../include/bitstream.h"
#include "../include/idct.h"
#include "../include/jpeg_reader.h"
#include "../include/decomp.h"
#include "../include/zigzag.h"
#include "../include/quant_inv.h"
#include "../include/color.h"



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

    float **table_cos = cos_table();
    struct bitstream *stream = get_bitstream(jpeg);
    uint8_t *quant_table = get_quantization_table(jpeg,0);
    int32_t *bloc = trad_bloc(stream, jpeg);
    int32_t ** frequence = quant_inv(bloc,quant_table);
    float **tab = naive_idct(frequence, table_cos);
    RGB **gris = ycbcr_to_gris(tab); 
    pixels_to_ppm(gris,8,8,1,argv[1]);

    /*free ALAIDE
    /*TODO
        get bitstream du jpeg pour travailler dessus
        Faut il extraire en avance toutes les tables ou on le fait que quand on
        en a besoin ?  
        decompression(jpeg)
    */
    close_jpeg(jpeg);
    return 0;
}
