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

    // On récupère toutes les tables et les données de l'image
    struct bitstream *stream = get_bitstream(jpeg);
    uint8_t noir_et_blanc = (get_nb_components(jpeg)==3)?0:1;

    uint8_t *quant_table_y = get_quantization_table(jpeg, 0);
    uint8_t *quant_table_c;
    if (noir_et_blanc == 0){
      quant_table_c = get_quantization_table(jpeg, 1);
    }
    //printf("%d\n", get_nb_quantization_tables(jpeg));
    uint16_t horizontal = get_image_size(jpeg,DIR_H);
    uint16_t vertical = get_image_size(jpeg,DIR_V);

    // Nombre de blocs arrondi au supérieur pour supporter les bordures
    uint8_t facteur_h = get_frame_component_sampling_factor(jpeg,DIR_H,0);
    uint8_t facteur_v = get_frame_component_sampling_factor(jpeg,DIR_V,0);
    uint16_t nb_mcu_h = ceil(((float) horizontal)/(8*facteur_h));
    uint16_t nb_mcu_v = ceil(((float) vertical)/(8*facteur_v));
    uint8_t mcu_lignes = 8*facteur_v;
    uint8_t mcu_colonnes = 8*facteur_h;
    uint16_t horizontal_fictif = 8*nb_mcu_h*facteur_h;
    uint16_t vertical_fictif = 8*nb_mcu_v*facteur_v;
    //printf("h_fic = %i, v_fic = %i\n", horizontal_fictif, vertical_fictif);
    //printf("horizontal = %i, vertical = %i", horizontal, vertical);

    // Création de l'image avec les blocs traduits
    RGB **big_tab = init_RGB(vertical_fictif, horizontal_fictif+16);
    int top_left_x = 0;
    int top_left_y = 0;
    int16_t *prec_y = calloc(1,sizeof(int16_t));
    int16_t *prec_cb = calloc(1,sizeof(int16_t));
    int16_t *prec_cr = calloc(1,sizeof(int16_t));
    //int16_t ** quant_y;// = init_mcu_quant(mcu_lignes, mcu_colonnes);
    //int16_t ** quant_cb;// = init_mcu_quant(mcu_lignes, mcu_colonnes);
    //int16_t ** quant_cr;// = init_mcu_quant(mcu_lignes, mcu_colonnes);
    //float ** freq_y;// = init_mcu_freq(mcu_lignes, mcu_colonnes);
    //float ** freq_cb;// = init_mcu_freq(mcu_lignes, mcu_colonnes);
    //float ** freq_cr;// = init_mcu_freq(mcu_lignes, mcu_colonnes);
    for (int i = 0; i< nb_mcu_v; i++){
       top_left_x = 0;
       for (int j = 0; j< nb_mcu_h;j++){
	 RGB ** mon_mcu;
	 if (noir_et_blanc == 0){
	   mon_mcu = trad_mcu(stream,jpeg,prec_y,prec_cb,prec_cr,facteur_h,facteur_v,mcu_lignes,mcu_colonnes,quant_table_y,quant_table_c);
	 }
	 else{
	   mon_mcu = trad_mcu_noir_et_blanc(stream,jpeg,prec_y,quant_table_y);
	 }
	 write_big_rgb(big_tab, top_left_x, top_left_y, mon_mcu, mcu_lignes, mcu_colonnes);
	 top_left_x += mcu_colonnes;
       }
       top_left_y += mcu_lignes;
    }

pixels_to_ppm(big_tab,horizontal,vertical,noir_et_blanc,argv[1]);
free(prec_y);
free(prec_cb);
free(prec_cr);

for(int i =0;i<vertical_fictif;i++){
  free(big_tab[i]);
     }
     free(big_tab);
      //struct image image = trad_image(stream, jpeg, nb_bloc_h, nb_bloc_v,0);

      // Initialisation des tableaux intermédiaires
      // Init image quant
     /* int16_t ****image_quant_y = init_image_quant(nb_bloc_v, nb_bloc_h);
      int16_t ****image_quant_cb = init_image_quant(nb_bloc_v, nb_bloc_h);
      int16_t ****image_quant_cr = init_image_quant(nb_bloc_v, nb_bloc_h);

      // Init image freq
      float ****image_freq_y = init_image_freq(nb_bloc_v, nb_bloc_h);
      float ****image_freq_cb = init_image_freq(nb_bloc_v, nb_bloc_h);
      float ****image_freq_cr = init_image_freq(nb_bloc_v, nb_bloc_h);

      // On passe chachun des blocs de l'image dans la chaine de modification
     // for (int i = 0; i<nb_bloc_v;i++){
	       for (int j =0; j<nb_bloc_h;j++){

          // Quantization et zigzag
	   //    image_quant_y[i][j] = quant_inv(image.y[i][j], quant_table_y);
           image_quant_cb[i][j] = quant_inv(image.cb[i][j], quant_table_c);
           image_quant_cr[i][j] = quant_inv(image.cr[i][j], quant_table_c);
          // iDCT
	//         image_freq_y[i][j] = idct(image_quant_y[i][j]);//naive_idct(image_quant[i][j], table_cos);//idct(image_quant[i][j]);naive_idct(image_quant[i][j], table_cos);
           image_freq_cb[i][j] = idct(image_quant_cb[i][j]);
           image_freq_cr[i][j] = idct(image_quant_cr[i][j]);

	        }
      }

      // On transforme les tableaux de fréquences 4D en 2D
    //  float ** array_y = bloc2array(image_freq_y, nb_bloc_h, nb_bloc_v);
      float ** array_cb = bloc2array(image_freq_cb, nb_bloc_h, nb_bloc_v);
      float ** array_cr = bloc2array(ige_freq_cr, nb_bloc_h, nb_bloc_v);

      // Init RGB
     // struct RGB **image_RGB = malloc(vertical_fictif*sizeof(struct RGB *));
      for (int i =0; i<vertical_fictif; i++){
	       image_RGB[i] = malloc(horizontal_fictif*sizeof(struct RGB));
      }

      // On utilise les trois arrays pour obtenir le tableau en RGB
      //for (int i=0; i<vertical_fictif; i++){
	       for (int j=0; j<horizontal_fictif; j++){
	          image_RGB[i][j] = ycbcr_to_rgb(array_y[i][j], array_cb[i][j], array_cr[i][j]);
	       }
      }

      // On transforme le tableau de blocs en tableau simple, les bordures en trop ne sont pas encore tronquées
      ///struct RGB **immondice = bloc2array(image_gris, nb_bloc_h, nb_bloc_v);

      // On transforme le tableau en image de pixels en ignorant les bordures en trop pour revenir à l'image originale
      pixels_to_ppm(image_RGB,horizontal,vertical,noir_et_blanc,argv[1]);
    }else{
      struct image image = trad_image(stream, jpeg, nb_bloc_h, nb_bloc_v,1);

      // Initialisation des tableaux intermédiaires
      // Init image quant
      int16_t ****image_quant_y = init_image_quant(nb_bloc_v, nb_bloc_h);
      float ****image_freq_y = init_image_freq(nb_bloc_v, nb_bloc_h);
      for (int i = 0; i<nb_bloc_v;i++){
	       for (int j =0; j<nb_bloc_h;j++){

          // Quantization et zigzag
	         image_quant_y[i][j] = quant_inv(image.y[i][j], quant_table_y);

          // iDCT
	         image_freq_y[i][j] = idct(image_quant_y[i][j]);//naive_idct(image_quant[i][j], table_cos);//idct(image_quant[i][j]);naive_idct(image_quant[i][j], table_cos);
	       }
      }
      float ** array_y = bloc2array(image_freq_y, nb_bloc_h, nb_bloc_v);
      struct RGB **image_RGB = malloc(vertical_fictif*sizeof(struct RGB *));
      for (int i =0; i<vertical_fictif; i++){
	       image_RGB[i] = malloc(horizontal_fictif*sizeof(struct RGB));
      }
      image_RGB = ycbcr_to_gris(array_y,horizontal_fictif,vertical_fictif);
      pixels_to_ppm(image_RGB,horizontal,vertical,1,argv[1]);
  }*/
    // 1789 tmtc
    //
    /*for(int i =0;i<vertical;i++){
        free(immondice[i]);
    }
    free(immondice);
    for(int i =0;i<nb_bloc_v;i++){
        for(int j=0;j<nb_bloc_h;j++){
            for(int a = 0;a<8;a++){
                free(image_gris[i][j][a]);
                free(image_freq[i][j][a]);
                free(image_quant[i][j][a]);
            }
            free(image_gris[i][j]);
            free(image_freq[i][j]);
            free(image_quant[i][j]);
            free(image[i][j]);
        }
        free(image_gris[i]);
        free(image_freq[i]);
        free(image_quant[i]);
        free(image[i]);
    }
    free(image_gris);
    free(image_freq);
    free(image_quant);
    free(image);



*/
    close_jpeg(jpeg);
    return 0;
}
