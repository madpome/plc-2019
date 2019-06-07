#include "../include/notre_jpeg_reader.h"


/* Duplique la chaine passée en paramètre */
char *dup(const char *chaine){
    char *new = malloc(sizeof(char)*strlen(chaine)+1);
    strcpy(new,chaine);
    return new;
}

/* Lecture du marqeur APP0 */
void parse_APP0(struct bitstream *stream){
    uint32_t bits = 0;
    read_bitstream(stream, 16, &bits, 0);
    uint32_t longueur = bits;
    bits = 0;
    int i = 0;
    char c;
    char *JFIF = "JFIF";

  // Lecture de JFIF
    while (i<5){
        read_bitstream(stream,8, &bits, 0);
        c = bits;
        bits = 0;
        if (JFIF[i] != c){
            break;
        }
        i++;
    }
    longueur -= 7;

    // Donnéees non traitées
    while (longueur != 0){
        read_bitstream(stream, 8, &bits, 0);
        longueur--;
    }
}


/* Lecture du marqueur COM */
void parse_com(struct bitstream *stream){
    uint32_t bits = 0;
    read_bitstream(stream, 16, &bits, 0);
    uint32_t longueur = bits - 2;
    bits = 0;

    // Commentaires de l'image
    while (longueur != 0){
        read_bitstream(stream, 8, &bits, 0);
        longueur--;
    }
}


/* Lecteur du marqueur DQT */
void parse_dqt(struct bitstream *stream,uint8_t **tables, uint8_t *compteur){
    uint32_t bits = 0;
    read_bitstream(stream, 16, &bits, 0);
    uint32_t longueur = bits - 2;
    bits = 0;
    uint8_t *res = malloc(64*sizeof(uint8_t));
    while (longueur != 0){

        // Précision
        read_bitstream(stream, 4, &bits, 0);
        uint8_t precision = bits;
        if (precision == 1){
            fprintf(stderr, "Précision non gérée dans notre cas\n");
            exit(0);
        }
        bits = 0;

        // Indice
        read_bitstream(stream, 4, &bits, 0);
        uint32_t indice = bits;
        bits = 0;
        longueur--;

        free(tables[indice]);

        // Lecture de la table de quantification
        int i = 0;
        while (i<64){
            read_bitstream(stream, 8, &bits, 0);
            res[i] = bits;
            bits = 0;
            i++;
            longueur --;
        }
        tables[indice] = res;
        *compteur+=1;
    }
}

/* Lecture du marqueur SOF0 */
void parse_SOF0(struct bitstream *stream, struct donnees *donnees){
    uint32_t bits = 0;
    read_bitstream(stream, 16, &bits, 0);
    uint32_t longueur = bits - 2;
    bits = 0;

    read_bitstream(stream, 8, &bits, 0);
    donnees->precision = bits;
    bits = 0;
    longueur--;

    read_bitstream(stream, 16, &bits, 0);
    donnees->hauteur = bits;
    bits = 0;
    longueur-=2;

    read_bitstream(stream, 16, &bits, 0);
    donnees->largeur = bits;
    bits = 0;
    longueur-=2;

    read_bitstream(stream, 8, &bits, 0);
    donnees->nb_composantes = bits;
    bits = 0;
    longueur--;

    struct composante *tab_comp = malloc(donnees->nb_composantes \
                                         *sizeof(struct composante));

    // Lecture du facteur d'échantillonage pour chaque composante
    for (uint32_t i =0; i<donnees->nb_composantes; i++){
        struct composante comp;
        read_bitstream(stream, 8, &bits, 0);
        comp.identifiant = bits;
        bits = 0;
        longueur--;

        read_bitstream(stream, 4, &bits, 0);
        comp.facteur_h = bits;
        bits = 0;

        read_bitstream(stream, 4, &bits, 0);
        comp.facteur_v = bits;
        bits = 0;
        longueur--;

        read_bitstream(stream, 8, &bits, 0);
        comp.table_quant = bits;
        bits = 0;
        longueur--;

        comp.component = i;
        tab_comp[i] = comp;
    }
    donnees->composantes = tab_comp;
}


/* Lecture du marqueur DHT */
void parse_DHT(struct bitstream *stream, struct huff_table **table_AC,
               struct huff_table **table_DC, uint8_t *compteur,
               uint8_t *taille_AC, uint8_t *taille_DC){
    uint32_t bits = 0;
    read_bitstream(stream, 16, &bits, 0);
    uint32_t longueur = bits - 2;
    bits = 0;
    uint16_t bits_lus;

    // Lecture de la table de huffman
    while (longueur != 0){

        // Bits qui valent 0
        read_bitstream(stream, 3, &bits, 0);
        if (bits != 0){
            fprintf(stderr, "Error reading huffman\n");
            exit(0);
        }
        // Type DC / AC
        read_bitstream(stream, 1, &bits, 0);
        uint8_t type = bits;
        bits =0;
        read_bitstream(stream, 4, &bits, 0);
        uint8_t indice = bits;
        bits =0;
        longueur--;

        struct huff_table *table = load_huffman_table(stream, &bits_lus);
        if (type){
            free(table_AC[indice]);
            table_AC[indice] = table;
            *taille_AC+=1;
        }
        else{
            free(table_DC[indice]);
            table_DC[indice] = table;
            *taille_DC+=1;
        }
        longueur-=bits_lus;
        *compteur+=1;
    }
}


/* Lecture du marqueur SOS */
void parse_SOS(struct bitstream *stream, struct bitstream **image,
               struct donnees *donnees){

    uint32_t bits = 0;
    read_bitstream(stream, 16, &bits, 0);
    uint32_t longueur = bits -2;
    bits = 0;

    read_bitstream(stream,8,&bits,0);
    if (bits != donnees->nb_composantes){
        fprintf(stderr, "Nombre de composantes invalide\n");
        exit(0);
    }
    bits = 0;
    longueur--;

    // Lecture des indices de Huffman pour chaque composante
    for (int i =0; i<donnees->nb_composantes;i++){
        read_bitstream(stream, 8, &bits,0);
        uint8_t id = bits;
        bits =0;
        longueur--;

        read_bitstream(stream, 4, &bits, 0);
        uint8_t id_dc = bits;
        bits =0;

        read_bitstream(stream, 4, &bits, 0);
        uint8_t id_ac = bits;
        bits = 0;
        longueur--;
        for (int j =0; j<donnees->nb_composantes; j++){
            if (donnees->composantes[j].identifiant == id){
	              donnees->composantes[j].ind_huffman_dc = id_dc;
	              donnees->composantes[j].ind_huffman_ac = id_ac;
            }
        }
    }
    // Utile au mode progressif donc pas utile ici
    read_bitstream(stream, 24, &bits, 0);
    bits = 0;
    *image = stream;
}


/* Lit l'entète en fonction des marqueur et renvoie le stream associé à l'image */
struct jpeg_desc *read_jpeg(const char *filename){

    struct jpeg_desc *jpeg = malloc(sizeof(struct jpeg_desc));

    // Initialisation du jpeg
    jpeg->filename = dup(filename);
    struct bitstream *stream = create_bitstream(filename);
    uint16_t marqueur = 0;
    uint8_t flag=0;
    uint32_t bits = 0;
    uint8_t compteur_q = 0;
    uint8_t compteur_h = 0;
    jpeg->taille_AC = 0;
    jpeg->taille_DC = 0;

    uint8_t **tables = calloc(4, sizeof(uint8_t *));
    struct huff_table **table_AC = calloc(4, sizeof(struct huff_table *));
    struct huff_table **table_DC = calloc(4, sizeof(struct huff_table *));
    struct donnees *donnees = malloc(sizeof(struct donnees));
    struct bitstream **image = malloc(sizeof(struct bitstream *));

    // On lit le token de début d'image
    read_bitstream(stream, 16, &bits, 0);
    if (bits != 0xffd8){
        fprintf(stderr,"Error reading Start of Image\n");
        exit(0);
    }

    // flag = fin de l'entête
    while (!flag){

        // On lit un marqueur sur 2 octets
        read_bitstream(stream, 16, &bits, 0);
        marqueur = bits;
        bits = 0;

        // On switch en fonction du marqueur lu
        switch(marqueur){
            case 0xffe0:
                parse_APP0(stream);
                break;
            case 0xfffe:
                parse_com(stream);
                break;
            case 0xffdb:{
                parse_dqt(stream,tables,&compteur_q);
                jpeg->tables_quantification = tables;
                break;
            }
            case 0xffc4:{
                parse_DHT(stream, table_AC, table_DC, &compteur_h,
                          &jpeg->taille_AC, &jpeg->taille_DC);

                jpeg->table_AC = table_AC;
                jpeg->table_DC = table_DC;
                break;
            }
            case 0xffc0:{
                parse_SOF0(stream,donnees);
                jpeg->donnees = donnees;
                break;
            }
            case 0xffda:{
                parse_SOS(stream,image,donnees);
                jpeg->image = image;
                flag = 1;
                break;
            }
            default:
                printf("Ce marqueur n'est pas géré par notre décodeur\n");
        }
    }
    jpeg->nb_quant_table = compteur_q;
    jpeg->nb_huff_table = compteur_h;
    return jpeg;
}


/* Récupère le stream de l'image */
struct bitstream *get_bitstream(const struct jpeg_desc *jpeg){
    return *(jpeg->image);
}

/* Récupère le nom du fichier */
char *get_filename(const struct jpeg_desc *jpeg){
    return jpeg->filename;
}

/* Récupère le nombre de tables de quantification */
uint8_t get_nb_quantization_tables ( const struct jpeg_desc *jpeg){
    return jpeg->nb_quant_table;
}

/* Récupère le nopmbre de tables de Huffman */
uint8_t get_nb_huff_tables(const struct jpeg_desc *jpeg){
    return jpeg->nb_huff_table;
}

/* Récupère les tables de quantification */
uint8_t *get_quantization_table(const struct jpeg_desc *jpeg, uint8_t index){
    return jpeg->tables_quantification[index];
}

/* Récupère les talbes de Huffman */
struct huff_table * get_huffman_table(const struct jpeg_desc *jpeg,
                                      enum acdc acdc, uint8_t index){
    if (acdc){
        return jpeg->table_AC[index];
    }
    else{
        return jpeg->table_DC[index];
    }
}

/* Récupère la taille de l'image */
uint16_t get_image_size(struct jpeg_desc *jpeg, enum direction dir){
    if (dir){
        return jpeg->donnees->hauteur;
    }
    else{
        return jpeg->donnees->largeur;
    }
}

/* Récupère le nombre de composants */
uint8_t get_nb_components(const struct jpeg_desc *jpeg){
    return jpeg->donnees->nb_composantes;
}

/* Récupère l'identifiant des composants YCbCr */
uint8_t get_frame_component_id(const struct jpeg_desc *jpeg,
                               uint8_t frame_comp_index ){

    return jpeg->donnees->composantes[frame_comp_index].identifiant;
}

/* Récupère les sampling factors */
uint8_t get_frame_component_sampling_factor(const struct jpeg_desc *jpeg,
                                            enum direction dir,
                                            uint8_t frame_comp_index ){
    if (!dir){
        return jpeg->donnees->composantes[frame_comp_index].facteur_h;
    }
    else{
        return jpeg->donnees->composantes[frame_comp_index].facteur_v;
    }
}

/* Récupère l'index d'un composant dans la table de quantification */
uint8_t get_frame_component_quant_index(const struct jpeg_desc *jpeg,
                                        uint8_t frame_comp_index ){
    return jpeg->donnees->composantes[frame_comp_index].table_quant;
}


/* Récupère l'index d'un composant dans la table de huffman */
uint8_t get_frame_component_huffman_index(const struct jpeg_desc *jpeg,
                                          uint8_t frame_comp_index,
                                          enum acdc acdc){
    if (acdc){
        return jpeg->donnees->composantes[frame_comp_index].ind_huffman_ac;
    }
    else{
        return jpeg->donnees->composantes[frame_comp_index].ind_huffman_dc;
    }
}


/* Ferme le bitstream et libère la mémoire allouée */
void close_jpeg(struct jpeg_desc *jpeg){
    for (int i=0; i<jpeg->nb_quant_table;i++){
        free(jpeg->tables_quantification[i]);
    }
    free(jpeg->tables_quantification);

    free(jpeg->donnees->composantes);
    free(jpeg->donnees);

    for (int i =0; i<jpeg->taille_AC; i++){
        free_huffman_table(jpeg->table_AC[i]);
    }
    free(jpeg->table_AC);

    for (int i=0; i<jpeg->taille_DC;i++){
        free_huffman_table(jpeg->table_DC[i]);
    }
    free(jpeg->table_DC);

    close_bitstream(*(jpeg->image));
    free(jpeg->image);

    free(jpeg->filename);

    free(jpeg);

}
