#include "../include/bitstream.h"
struct bitstream{
    //Fichier lié au bitstream
    FILE * f;
    //Position de la tête de lecture en BYTE, on se met AVANT le byte à lire
    long int current;
    // valeur 1 byte avant current
    uint8_t prec;
    //Position de la tête de lecture DANS le byte donc en BIT
    int cur_bit;
    //Position de la FIN du fichier
    long int end;
};

//Créer le bitstream
struct bitstream * create_bitstream(const char * filename){
    printf("opening %s\n",filename);
    FILE * f = fopen(filename, "r");
    if(f==NULL){
        return NULL;
    }
    struct bitstream *bit = calloc(1, sizeof(struct bitstream));
    bit->f = f;
    bit->current = 0;
    bit->cur_bit = 0;
    bit->prec = 0;
    fseek(f, 0, SEEK_END);
    bit->end = ftell(f);
    fseek(f,0,SEEK_SET);
    printf("END : %ld\n",bit->end);
    return bit;
}

void print_bin(uint32_t a){
    for(int i =0;i<32;i++){
        printf("%d",(a>>(32-i-1) & 1)==1);
    }
    printf("\n");
}

void close_bitstream ( struct bitstream * stream ){
    fclose(stream->f);
    free(stream);
}
//Met le bit de dest à la position pos à bit
void set_bit(uint32_t *dest, int pos, uint32_t bit){
    if(bit != 0){
        *dest = *dest | (1<<pos);
    }else{
        uint32_t a = 0;
        *dest = *dest & (~(a | 1<<pos));
    }
}

bool end_of_bitstream ( struct bitstream * stream ){
    return (stream->current == stream->end)?true:false;
}

//renverse les bits de a de 0 à taille
uint32_t reverse_opt(uint32_t a, int taille){
    uint32_t b=0;
    for(int i =0; i<taille;i++){
        set_bit(&b, i, a>>(taille-i-1) & 1);
    }
    return b;
}

//Lit que sur 16 lui
uint8_t read_bitstream_bis(struct bitstream *stream ,uint8_t nb_bits , uint32_t *dest, bool discard_byte_stuffing, int numero){
    if(fseek(stream->f, stream->current, SEEK_SET) !=0){
        //erreur
        exit(-1);
    }
    //d1 est le premier byte, celui qu'on utilise, d2 est lu en prévision
    uint8_t *d1 = calloc(1, sizeof(uint8_t));
    uint8_t *d2 = calloc(1, sizeof(uint8_t));
    int n = fread(d1, 1, 1, stream->f);
    //Si je vois qu'il vaut et que prec vaut FF alors je le saute si on discard
    if(discard_byte_stuffing==true && stream->prec == 0xFF && *d1 == 0x00){
        n = fread(d1,1,1,stream->f);
        stream->current+=n;
        if(n == 0) return 0;
        stream->cur_bit = 0;
    }
    int n2 =fread(d2,1,1,stream->f);
    //position du bit à lire (de gauche a droite) et commence a 0
    //8 vaut 1000
    int position = stream->cur_bit;
    //Nombre de bit effectiviement lu
    int actually_read = 0;
    //permet de distinguer les cas
    int a_lire = 1;
    int i;
    for(i =0;i<nb_bits;i++){
        if(position ==8){
            //Cas ou on a pas assez de bit dans le d2 dont on doit relire encore un byte
            if(a_lire == 2){
                break;
            }
            //cas ou c'était le dernier byte
            if(stream->current == stream->end){
                break;
            }
            //Cas ou on passe juste au byte d2
            a_lire = 2;
            if(discard_byte_stuffing == true && *d1 == 0xFF && *d2 == 0x00){
                //il faut sauter les 2 prochains bytes
                n2 = fread(d2, 1, 1, stream->f);
                stream->current+=n2;
                if(n2 == 0) break;
            }
            position = 0;
        }
        set_bit(dest,i, ((a_lire==1)?*d1:*d2)>>(7-position) & 1);
        actually_read++;
        //Si on est en fin de byte on passe au suivant
        if(position == 7){
            stream->current++;
        }
        position++;
    }
    //On doit lire au plus encore 1 byte
    //Je regarde que si numero == 2 car sinon les bytes a lire seront lu dans le deuxiee
    if(numero == 2 && position == 8 && a_lire == 2 && i != nb_bits){
        a_lire = 3;
        n = fread(d1,1,1, stream->f);
        if(n != 0){
            if(discard_byte_stuffing==true && *d2 == 0xFF && *d1 == 0x00){
            n = fread(d1,1,1,stream->f);
            stream->current+=n;
            stream->cur_bit = 0;
            }
            position = 0;
            //Je lis les bits manquant
            for(int j = i; j<nb_bits; j++){
                set_bit(dest, j, *d1>>(7-position) & 1);
                actually_read++;
                position++;
            }
        }
    }
    //Si j'étais en train de lire le 2eme byte
    if(a_lire == 2){
        if(position == 8){
            //Si je suis arrivé a la fin, c'est lui le dernier
            stream->prec = *d2;
        }else{
            //sinon d1
            stream->prec = *d1;
        }
        //idem raisonnement pour le reste
    }else if(position == 8 && a_lire == 1){
        stream->prec = *d1;
    }else if(a_lire ==3){
        stream->prec = *d2;
    }
    //JE le renverse car pas dans le bon sens
    *dest = reverse_opt(*dest, actually_read);
    //je met a jour cur_bit
    stream->cur_bit = (stream->cur_bit + actually_read)%8;
    free(d1);
    free(d2);
    return actually_read;

}

//discard_byte_stuffing : si on lit 00 et qu'il est precedé de FF et que c'est
//a true, alors on ignore
uint8_t read_bitstream (struct bitstream *stream ,uint8_t nb_bits , uint32_t *dest, bool discard_byte_stuffing ){
    *dest = 0;
    uint32_t a = 0;
    if(nb_bits>32){
        return 0;
    }
    int n = read_bitstream_bis(stream,(nb_bits<=16)?nb_bits:16,dest,discard_byte_stuffing,1);

    if(nb_bits>16){
        printf("2\n");
        int n2 = read_bitstream_bis(stream,nb_bits-16,&a,discard_byte_stuffing,2);
        *dest = (*dest) << n2 | a;
        n+=n2   ;
    }

    //retourne_moi_ta_mere(dest, n);
    /*printf("nb_bits = %d, discard_byte_stuffing = %d n = %d\n",nb_bits,discard_byte_stuffing,n);
    printf("curr = %ld prec= %x cur_bit = %d end = %ld\n",stream->current,  stream->prec, stream->cur_bit, stream->end);
    print_bin(*dest);
    printf("%x\n",*dest);*/
    return n;
}
/*
int main(int argc, char **argv){
    FILE *f = fopen("toto.txt","w");
    uint16_t a = 0;
    a = 0xFF11;
    fwrite(&a,1, 2, f);
    a = 0xFF00;
    fwrite(&a,1, 2, f);
    fclose(f);
    struct bitstream* stream = create_bitstream("toto.txt");
    uint32_t b = 0;
    printf("eof = %ld\n", stream->end);
    while(end_of_bitstream(stream) == false){
        a = read_bitstream(stream, 8 , &b, true);
        printf("%d\n",a);
        print_bin(b);
        printf("current = %ld, cur_bit = %d, prec = %d\n",stream->current, stream->cur_bit, stream->prec);
    }
    return 1;
}*/
/* Optionnel ! */
//extern void skip_bitstream_until ( struct bitstream *stream ,uint8_t byte);
