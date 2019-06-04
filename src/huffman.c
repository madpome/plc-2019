#include "../include/huffman.h"

typedef struct Arbre{
    uint8_t val;
    struct Arbre *fg;
    struct Arbre *fd;
}Arbre;
struct huff_table{
    //Cet arbre ne peut jamais être NULL;
    uint8_t nb_elem;
    Arbre *arbre;
};


char * gimme_string(int taille, uint16_t compteur){
    //Renvoie la chaine de 0 et 1 associé au noeud de profondeur taille et de numéro compteur de gauche a droite;
    char *s = calloc((taille+1),sizeof(char));
    uint16_t a = 1;
    //Il faudra lire le code "a l'envers"
    for(int i = 0;i<compteur;i++){
        s[i] = ((compteur & a) == a)?'1':'0';
        a = a <<1;
    }
    s[compteur]= '\0';
    return s;
}
void add_to_arbre(Arbre *arbre, char *chemin, uint8_t value){
    Arbre *curr = arbre;
    Arbre *next = NULL;
    int n = strlen(chemin);
    for(int i =0;i<n;i++){
        next = calloc(1,sizeof(Arbre));
        if(chemin[n-1-i] == '0'){
            curr->fg = next;
        }else{
            curr->fd = next;
        }
        curr = next;
    }
    next->val = value;
}
uint16_t donne_debut(uint8_t *nb_elem, uint16_t profondeur){
    //Codes commencent à 0
    //Au max par étage, on a 2^h noeuds
    //Si il y a n1 code de profondeur h1, on commence au moins à n1*2^(h2-h1)
    uint16_t n = 0;
    for(int i =0;i<(profondeur-1);i++){
        for(int j = 0;j<nb_elem[i];j++){
            n+=(2<<(profondeur-j));
        }
    }
    return n;
}
struct huff_table * load_huffman_table(struct bitstream *stream, uint16_t *nb_byte_read){
    struct huff_table *table=calloc(1,sizeof(struct huff_table));
    *nb_byte_read = 0;
    uint8_t nb_elem[16];
    int lu=0;
    table->nb_elem = 0;
    uint32_t pre_lecture = 0;
    for(int i = 0;i<16;i++){
        lu = read_bitstream(stream, 8, &pre_lecture, false);
        if(lu == 8){
            pre_lecture = pre_lecture>>24;
            nb_elem[i] = (uint8_t) pre_lecture;
            pre_lecture = 0;
            table->nb_elem+= nb_elem[i];
            (*nb_byte_read)++;
        }else{
            fprintf(stderr, "y'a un pb doc 1\n");
            exit(-1);

        }//sinon c'est une erreur et je sais pas
    }
    //On commence à lire les caractères
    uint8_t c = 0;
    pre_lecture = 0;
    uint16_t debut = 0;
    uint16_t nombre = 0;
    uint16_t profondeur = 1;
    for(int i = 0;i<table->nb_elem;i++){
        lu = read_bitstream(stream, 8, &pre_lecture, false);
        if(lu!=8){
            fprintf(stderr, "y'a un pb doc 2\n");
            exit(-1);
        }
        c = (uint8_t) (pre_lecture>>24);
        //On se met au premier truc où on a effectiviement un truc a coder
        if(nombre == nb_elem[profondeur-1]){
            while(nb_elem[profondeur-1] == 0){
                profondeur++;
            }
            debut = donne_debut(nb_elem, profondeur);
            nombre = 0;
        }
        char *chem = gimme_string(profondeur,debut);
        add_to_arbre(table->arbre,chem,c);
        debut++;
        nombre++;
        free(chem);
    }
    return table;
}
int8_t next_huffman_value(struct huff_table *table, struct bitstream *stream){
    Arbre *arbre = table->arbre;
    int feuille = 0;
    uint32_t lire = 0;
    while(feuille == 0){
        if(arbre->fg == NULL && arbre->fd == NULL){
            return arbre->val;
        }
        if(read_bitstream(stream, 1, &lire, false) != 8){
            fprintf(stderr, "y'a un pb doc 3\n");
            exit(-1);
        }
        lire = lire >>31;
        if(lire == 1){
            arbre = arbre->fd;
        }else{
            arbre = arbre->fg;
        }
        lire = 0;
    }
    fprintf(stderr,"bizarre doc\n");
    return -1;
}

/*int8_t next_huffman_value_count(struct huff_table *table,
                                 struct bitstream *stream,
                                 uint8_t *nb_bits_read);
*/
void free_arbre(Arbre *arbre){
        if(arbre->fg != NULL){
            free_arbre(arbre->fg);
        }
        if(arbre->fd != NULL){
            free_arbre(arbre->fd);
        }
        free(arbre);
}

void free_huffman_table(struct huff_table *table){
    free_arbre(table->arbre);
}
