#include "../include/bitstream.h"
struct bitstream{
    //Fichier lié au bitstream
    FILE * f;
    // valeur 1 byte avant current
    uint8_t cur;
    bool prec_is_ff;
    bool never_read;
    //Position de la tête de lecture DANS le byte donc en BIT
    int cur_bit;
};

//Créer le bitstream
struct bitstream * create_bitstream(const char * filename){
  //printf("opening %s\n",filename);
    FILE * f = fopen(filename, "r");
    if(f==NULL){
        return NULL;
    }
    struct bitstream *bit = calloc(1, sizeof(struct bitstream));
    bit->f = f;
    bit->cur_bit = 8;
    bit->prec_is_ff = false;
    bit->never_read = true;
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
    return feof(stream->f);
}

//renverse les bits de a de 0 à taille
uint32_t reverse_opt(uint32_t a, int taille){
    uint32_t b=0;
    for(int i =0; i<taille;i++){
        set_bit(&b, i, a>>(taille-i-1) & 1);
    }
    return b;
}

uint8_t read_8(struct bitstream *stream , uint8_t nb_bits, uint32_t *dest, bool discard_byte_stuffing){
  if(end_of_bitstream(stream)){
    printf("hoohoho");
    *dest =0;
    return 0;
  }
    if(stream->never_read == true){
        stream->never_read = false;
        stream->cur_bit = 0;
        fread(&(stream->cur),1,1,stream->f);
    }
    *dest = 0;
    int n =0;
    int actually_read = 0;
    while(stream->prec_is_ff == true && discard_byte_stuffing == true && stream->cur == 0x00){
        stream->cur_bit = 0;
        n = fread(&(stream->cur), 1, 1 ,stream->f);
        if(n == 0) return 0;
    }
    uint8_t new_cur_bit = stream->cur_bit;
    int k =((stream->cur_bit+nb_bits>8)?8:(stream->cur_bit+nb_bits));
    //printf("k = %d\n",k);
    for(int i = stream->cur_bit;i<k;i++){
        set_bit(dest,actually_read,stream->cur>>(7-i)& 1);
        actually_read++;
        new_cur_bit++;
    }
    stream->cur_bit = new_cur_bit;
    //forcement <=8
    //<8 que si actually_read == nb_bits

    if(actually_read != nb_bits || (stream->cur_bit == 8 && !end_of_bitstream(stream))){
        if(stream->cur == 0xFF){
	  //printf("tueueue\n");
            stream->prec_is_ff = true;
        }else{
            stream->prec_is_ff = false;
        }
        n = fread(&(stream->cur),1,1,stream->f);
        stream->cur_bit = 0;
        //print_bin(stream->cur);
        if (n == 0) return actually_read;
        while(stream->prec_is_ff == true && discard_byte_stuffing == true && stream->cur == 0x00){
            n = fread(&(stream->cur), 1, 1 ,stream->f);
            if(n == 0) return actually_read;
        }
        int o = actually_read;
        for(int i =0;i<(nb_bits-o);i++){
            set_bit(dest, actually_read, stream->cur>>(7-i) & 1);
            stream->cur_bit++;
	    actually_read++;
        }
    }
    

    return actually_read;
}

//discard_byte_stuffing : si on lit 00 et qu'il est precedé de FF et que c'est
//a true, alors on ignore
uint8_t read_bitstream (struct bitstream *stream ,uint8_t nb_bits , uint32_t *dest, bool discard_byte_stuffing ){
    *dest = 0;
    if(nb_bits>32){
        return 0;
    }
    uint32_t a = 0;
    int lu = 0;
    int bis = 0;
    for(int i = 0;i<nb_bits/8;i++){
        lu =read_8(stream,8, &a, discard_byte_stuffing);
        if (lu == 0) {
            break;
        }
        *dest =*dest | a<<bis;
        bis += lu;
        a = 0;
    }

    if(stream->never_read == true){
        stream->never_read = false;
        stream->cur_bit = 0 ;
        fread(&(stream->cur),1,1,stream->f);
        //printf("mdr j'ai lu\n");
    }
    if(nb_bits%8 != 0){
      //printf("diff de 0 frere %d\n",nb_bits%8);
        a = 0;
        lu = read_8(stream,nb_bits%8,&a, discard_byte_stuffing);
	if (lu == 0) {
            *dest = reverse_opt(*dest, bis);
            return bis;
        }
        *dest |= a<<bis;
        bis+=lu;
    }
    *dest = reverse_opt(*dest, bis);
    return bis;
}
/*
int main(int argc, char **argv){
    FILE *f = fopen("toto.txt","w");
    uint16_t a = 0;
    a = 0xFF;
    fwrite(&a,1, 1, f);
    a = 0x0;
    fwrite(&a,1, 1, f);
    fclose(f);
    struct bitstream* stream = create_bitstream("toto.txt");
    uint32_t b = 0;
    while(end_of_bitstream(stream) == false){  
      a = read_bitstream(stream, 7  , &b, true);
        printf("%d\n",a);
	printf("b: %x\n",b);
        printf("current = %x, cur_bit = %d, prec = %d\n",stream->cur, stream->cur_bit, stream->prec_is_ff);
    }
    return 1;
    }*/

/* Optionnel ! */
//extern void skip_bitstream_until ( struct bitstream *stream ,uint8_t byte);
