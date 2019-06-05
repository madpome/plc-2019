
#include "../include/bitstream.h"
struct bitstream{
    //Fichier lié au bitstream
    FILE * f;
    //Position de la tête de lecture en BYTE, on se met AVANT le byte à lire
    long int current;
    // valeur 2 byte avant current
    uint8_t prec;
    //Position de la tête de lecture DANS le byte donc en BIT
    int cur_bit;
    long int end;
};


struct bitstream * create_bitstream(const char * filename){
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
    return bit;
}
void close_bitstream ( struct bitstream * stream ){
    fclose(stream->f);
    free(stream);
}
//retourne les bits de a
uint16_t reverse(uint16_t a){
    uint16_t b = 0;
    for(int i =0;i<16;i++){
        b |= ((a & (1>>(15-i))) == (1>>(15-i)))?(1<<i):0;
    }
    return b;
}
void set_bit(uint32_t *dest, int pos, uint32_t bit){
    if(bit != 0){
        *dest = *dest | (1<<pos);
    }
}
bool end_of_bitstream ( struct bitstream * stream ){
    return (stream->end==stream->current)?true:false;
}
//Lit que sur 16 lui
uint8_t read_bitstream_bis(struct bitstream *stream ,uint8_t nb_bits , uint32_t *dest, bool discard_byte_stuffing, int numero){
    if(fseek(stream->f, stream->current, SEEK_SET) !=0){
        //erreur
        exit(-1);
    }
    uint8_t *d1 = calloc(1, sizeof(uint8_t));
    uint8_t *d2 = calloc(1, sizeof(uint8_t));
    int n = fread(d1, 1, 1, stream->f);
    if(discard_byte_stuffing==true && stream->prec == 0xFF && *d1 == 0x00){
        n = fread(d1,1,1,stream->f);
        stream->current+=1;
        stream->cur_bit = 0;
    }

    int n2 =fread(d2,1,1,stream->f);
    uint16_t d3 = *d1 | ((*d2) <<8);
    int position = stream->cur_bit;
    int actually_read = 0;
    int i;
    for(i =0;i<nb_bits;i++){
        //ici c'est le cas ou cur_bit > 0 et on lit plus de bits
        //Ou si on a pas assez de bits
        if(position == 8*(n+n2)){
            break;
        }
        if(position ==8 && discard_byte_stuffing == true && *d1 == 0xFF && *d2 == 0x00){
            //il faut sauter les 2 prochains bytes
            n2 = fread(d2, 1, 1, stream->f);
            stream->current+=n2;
            d3 = (*d2)<<8;
        }
        set_bit(dest, nb_bits-1-i, d3>>position & 1);
        actually_read++;
        if(position%7 == 0 && position !=0){
            stream->current++;
        }

        position++;
    }
    //On doit lire au plus encore 1 byte
    //mais on va en lire 2 juste pour que ce soit plus simple de verifier le 00
    if(numero == 2 && position == 16 && i != nb_bits){
        n += fread(d1,1,1, stream->f);
        if(discard_byte_stuffing==true && *d2 == 0xFF && *d1 == 0x00){
            n = fread(d1,1,1,stream->f);
            stream->current+=n;
            stream->cur_bit = 0;
        }
        position = 0;
        for(int j = i; j<nb_bits; j++){
            set_bit(dest, nb_bits-1-j, *d1>>position & 1);
            actually_read++;
            position++;
        }
    }

    if(position >=8 && position < 16){
        stream->prec = *d1;
    }else if(position >= 16){
        stream->prec = *d2;
    }
    if(actually_read != nb_bits){
        *dest = *dest>>(nb_bits-actually_read);
    }
    stream->cur_bit = (stream->cur_bit + actually_read)%8;
    //on se remet au bon endroit
    return actually_read;

}
//discard_byte_stuffing : si on lit 00 et qu'il est precedé de FF et que c'est
//a true, alors on ignore
uint8_t read_bitstream (struct bitstream *stream ,uint8_t nb_bits , uint32_t *dest, bool discard_byte_stuffing ){
    *dest = 0;
    uint32_t a = 0;
    int n = read_bitstream_bis(stream,(nb_bits<=16)?nb_bits:16,dest,discard_byte_stuffing,1);
    if(nb_bits>16){
        int n2 = read_bitstream_bis(stream,nb_bits-16,&a,discard_byte_stuffing,2);
        *dest = (*dest) << n2 | a;
        n+=n2;
    }
    return n;
}
void print_bin(uint32_t a){
    for(int i =0;i<32;i++){
        printf("%d",(a>>(32-i-1) & 1)==1);
    }
    printf("\n");
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

    while(end_of_bitstream(stream) == false){
        a = read_bitstream(stream, 5 , &b, true);
        printf("%d\n",a);
        print_bin(b);
        printf("current = %ld, cur_bit = %d, prec = %d\n",stream->current, stream->cur_bit, stream->prec);
    }
    return 1;
}*/
/* Optionnel ! */
//extern void skip_bitstream_until ( struct bitstream *stream ,uint8_t byte);
