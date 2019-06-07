#include "../include/huffman.h"
typedef struct Arbre{
    uint8_t val;
    struct Arbre *fg;
    struct Arbre *fd;
}Arbre;
struct huff_table{
    //Cet arbre ne peut jamais être NULL;
    uint8_t nb_elem;
    struct Arbre *arbre;
};

char * gimme_string(int taille, uint16_t compteur){
    //Renvoie la chaine de 0 et 1 associé au noeud de profondeur taille et de numéro compteur de gauche a droite;
    char *s = calloc((taille+1),sizeof(char));
    uint16_t a = 1;
    //Il faudra lire le code "a l'envers"
    for(int i = 0;i<taille;i++){
        s[taille-1-i] = ((compteur>>i & a) == a)?'1':'0';
    }
    s[taille]= '\0';
    return s;
}


void add_to_arbre(Arbre *arbre, char *chemin, uint8_t value){
    Arbre *curr = arbre;
    Arbre *next = NULL;
    int n = strlen(chemin);
    for(int i =0;i<n;i++){
        if(chemin[i] == '0'){
            if(curr->fg == NULL){
                next = calloc(1,sizeof(struct Arbre));
                next->fg =NULL;
                next->fd =NULL;
                curr->fg = next;
            }else{
                next = curr->fg;
            }
        }else{
            if(curr->fd == NULL){
                next = calloc(1,sizeof(struct Arbre));
                next->fg =NULL;
                next->fd =NULL;
                curr->fd = next;
            }else{
                next = curr->fd;
            }
        }
        curr = next;
    }
    curr->val = value;
}


uint16_t donne_debut(uint8_t *nb_elem, uint16_t profondeur){
    //Codes commencent à 0
    //Au max par étage, on a 2^h noeuds
    //Si il y a n1 code de profondeur h1, on commence au moins à n1*2^(h2-h1)
    uint16_t n = 0;
    for(int i =0;i<(profondeur-1);i++){
        for(int j = 0;j<nb_elem[i];j++){
            n+=(2<<(profondeur-i-2));
        }
    }
    return n;
}

struct huff_table * load_huffman_table(struct bitstream *stream, uint16_t *nb_byte_read){
    struct huff_table *table=calloc(1,sizeof(struct huff_table));
    table->arbre = calloc(1,sizeof(struct Arbre));
    (table->arbre)->fg = NULL;
    (table->arbre)->fd = NULL;
    int oct_total = 0;
    *nb_byte_read = 0;
    uint8_t nb_elem[16];
    int lu=0;
    table->nb_elem = 0;
    uint32_t pre_lecture = 0;
    for(int i = 0;i<16;i++){
        lu = read_bitstream(stream, 8, &pre_lecture, false);
        oct_total += lu;
        if(lu == 8){
            nb_elem[i] = 0;
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
        lu = read_bitstream(stream, 8, &pre_lecture, true);
        oct_total+=lu;
        if(lu!=8){
            fprintf(stderr, "y'a un pb doc 2 lu =%d, i = %d\n",lu,i);
            exit(-1);
        }
        //pre_lecture = reverse_opt(pre_lecture,8);
        c = (uint8_t) pre_lecture;
        //On se met au premier truc où on a effectiviement un truc a coder
        if(nombre == nb_elem[profondeur-1]){
            profondeur++;
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
        (*nb_byte_read)++;
        free(chem);
    }
    return table;
}
int8_t next_huffman_value(struct huff_table *table, struct bitstream *stream){
    Arbre *arbre = table->arbre;

    uint32_t lire = 0;
    while(0 == 0){
        if(arbre->fg == NULL && arbre->fd == NULL){
            return arbre->val;
        }
        if(read_bitstream(stream, 1, &lire, true) != 1){
            fprintf(stderr, "y'a un pb doc 3\n");
            exit(-1);
        }
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
    free(table);
}
/*
int main(int argc, char**argv){
    FILE * f = fopen("toto.txt","w");
    char c = 0;
    fwrite(&c, 1, 1, f);
    c= 3;
    fwrite(&c, 1, 1, f);
    c=2;
    fwrite(&c, 1, 1, f);
    c=0;
    for(int i =0;i<13;i++){
        fwrite(&c, 1, 1, f);
    }
    c = 'd';
    fwrite(&c, 1, 1, f);
    c ='e';
    fwrite(&c, 1, 1, f);
    c = 'a';
    fwrite(&c, 1, 1, f);
    c='c';
    fwrite(&c, 1, 1, f);
    c = 'b';
    fwrite(&c, 1, 1, f);
    c =0xD8;
    fwrite(&c, 1, 1, f);
    c = 0x0E;
    fwrite(&c, 1, 1, f);
    fclose(f);
    struct bitstream * stream = create_bitstream("toto.txt");
    uint16_t lu  =0;
    struct huff_table * huff = load_huffman_table(stream, &lu);
    printf("%c\n",next_huffman_value(huff,stream));
    printf("%c\n",next_huffman_value(huff,stream));
    printf("%c\n",next_huffman_value(huff,stream));
    printf("%c\n",next_huffman_value(huff,stream));
    printf("%c\n",next_huffman_value(huff,stream));
    close_bitstream(stream);
    free_huffman_table(huff);

    return 1;
}*/
