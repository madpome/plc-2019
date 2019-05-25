///#include "../include/bitstream.h"
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
    if(jpeg == NULL)
    {
        fprintf(stderr, "Problème lors de l'ouverture du fichier %s\n", argv[1]);
        return 1;
    }
    float **table_cos = cos_table();
    struct bitstream *stream = get_bitstream(jpeg);
    uint8_t *quant_table = get_quantization_table(jpeg,0);
    uint16_t horizontal = get_image_size(jpeg,DIR_H);
    uint16_t vertical = get_image_size(jpeg,DIR_V);
    uint16_t nb_bloc_h = horizontal/8;
    uint16_t nb_bloc_v = vertical/8;

    int16_t ***image = trad_image(stream, jpeg,nb_bloc_h,nb_bloc_v);
    int16_t ****image_quant = malloc(nb_bloc_h*sizeof(float ***));
    for (int i =0; i<nb_bloc_h;i++){
	image_quant[i] = malloc(nb_bloc_v*sizeof(float **));
    float ****image_freq = malloc(nb_bloc_h*sizeof(float ***));
    for (int i =0; i<nb_bloc_h;i++){
	image_freq[i] = malloc(nb_bloc_v*sizeof(float **));
      }
    struct RGB ****image_gris = malloc(nb_bloc_h*sizeof(float ***));
    for (int i =0; i<nb_bloc_h;i++){
      image_gris[i] = malloc(nb_bloc_v*sizeof(float **));
    }
    
    for (int j = 0; j<nb_bloc_v;j++){
	       for (int i =0; i<nb_bloc_h;i++)
	       {
	          image_quant[i][j] = quant_inv(image[i][j], quant_table);
	          image_freq[i][j] = naive_idct(image_quant[i][j], table_cos);
		  image_gris[i][j] = ycbcr_to_gris(image_freq[i][j]);
            /* Faut convertir en ppm */
	       }
    }

    /* Pour 1 seul bloc */
    // int16_t ** frequence = quant_inv(bloc,quant_table);
    // float **tab = naive_idct(frequence, table_cos);
    // RGB **gris = ycbcr_to_gris(tab);
    // pixels_to_ppm(gris,8,8,1,argv[1]);


    /*free ALAIDE */
    /*TODO
        get bitstream du jpeg pour travailler dessus
        Faut il extraire en avance toutes les tables ou on le fait que quand on
        en a besoin ?
        decompression(jpeg)
    */
    close_jpeg(jpeg);
    return 0;
}
