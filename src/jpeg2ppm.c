///#include "../include/bitstream.h"
#include "../include/idct.h"
#include "../include/jpeg_reader.h"
#include "../include/decomp.h"
#include "../include/zigzag.h"
#include "../include/quant_inv.h"
#include "../include/color.h"
#include "../include/initialisation_image.h"
#include "../include/mcu.h"


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

    struct bitstream *stream = get_bitstream(jpeg);

    // On lit le nombre de composantes pour savoir si on est en noir et blanc
    uint8_t noir_et_blanc = (get_nb_components(jpeg)==3)?0:1;

    // On récupère les indices des tables de quantification
    uint8_t indice_quant_y = get_frame_component_quant_index(jpeg, 0);
    uint8_t indice_quant_cb = get_frame_component_quant_index(jpeg, 1);
    uint8_t indice_quant_cr = get_frame_component_quant_index(jpeg, 2);

    // On récupère les tables de quantification
    uint8_t *quant_table_y = get_quantization_table(jpeg, indice_quant_y);
    uint8_t *quant_table_cb;
    uint8_t *quant_table_cr;
    if (noir_et_blanc == 0){
        quant_table_cb = get_quantization_table(jpeg, indice_quant_cb);
        quant_table_cr = get_quantization_table(jpeg, indice_quant_cr);

    }

    // Taille de l'image
    uint16_t horizontal = get_image_size(jpeg,DIR_H);
    uint16_t vertical = get_image_size(jpeg,DIR_V);

    // Facteurs de sampling
    uint8_t facteur_h = get_frame_component_sampling_factor(jpeg,DIR_H,
                                                            indice_quant_y);
    uint8_t facteur_v = get_frame_component_sampling_factor(jpeg,DIR_V,
                                                            indice_quant_y);

    // Nombre de MCU arrondi au supérieur pour supporter les bordures
    uint16_t nb_mcu_h = ceil(((float) horizontal)/(8*facteur_h));
    uint16_t nb_mcu_v = ceil(((float) vertical)/(8*facteur_v));

    // Nombre de lignes et de colonnes d'un MCU
    uint8_t mcu_lignes = 8*facteur_v;
    uint8_t mcu_colonnes = 8*facteur_h;

    // Taille de l'image arrondie au plus proche entier supérieur multiple de 8
    uint16_t horizontal_fictif = 8*nb_mcu_h*facteur_h;
    uint16_t vertical_fictif = 8*nb_mcu_v*facteur_v;


    // Création de l'image RGB finale
    RGB **big_tab = init_RGB(vertical_fictif, horizontal_fictif);

    // Cordonnées de notre position actuelle
    int top_left_x = 0;
    int top_left_y = 0;

    // On initialise les pointeurs vers les précédents DC de chaque composantes
    struct prec prec = {calloc(1, sizeof(int16_t)),
                        calloc(1, sizeof(int16_t)),
                        calloc(1, sizeof(int16_t))};


    /*int16_t *prec_y = calloc(1,sizeof(int16_t));
    int16_t *prec_cb = calloc(1,sizeof(int16_t));
    int16_t *prec_cr = calloc(1,sizeof(int16_t));
*/
    /* On traduit tous les MCU selon qu'ils soient en couleur ou en gris
       et on recopie chaque MCU traduit dans l'image RGB finale */
    for (int i = 0; i < nb_mcu_v; i++){
        top_left_x = 0;
        for (int j = 0; j < nb_mcu_h; j++){
	          RGB ** mon_mcu;
	           if (noir_et_blanc == 0){
	               mon_mcu = trad_mcu(stream, jpeg, prec,
                                    facteur_h, facteur_v,
                                    mcu_lignes, mcu_colonnes,
                                    quant_table_y, quant_table_cb,
                                    quant_table_cr);
	           } else {
	               mon_mcu = trad_mcu_noir_et_blanc(stream, jpeg,prec.y,
                                                  quant_table_y);
	           }
	           write_big_rgb(big_tab, top_left_x, top_left_y, mon_mcu,
                           mcu_lignes, mcu_colonnes);

             for (int k = 0; k < mcu_lignes; k++){
                free(mon_mcu[k]);
             }
             free(mon_mcu);
	           top_left_x += mcu_colonnes;
        }
        top_left_y += mcu_lignes;
    }

    // On transforme notre tableau de RGB en une image au format idoine
    pixels_to_ppm(big_tab, horizontal, vertical, noir_et_blanc, argv[1]);

    free(prec.y);
    free(prec.cb);
    free(prec.cr);

    for(int i =0;i<vertical_fictif;i++){
        free(big_tab[i]);
    }
    free(big_tab);

    close_jpeg(jpeg);
    return 0;
}
