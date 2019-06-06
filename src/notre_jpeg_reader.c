#include "../include/notre_jpeg_reader.h"

void parse_APP0(struct bitstream *stream){
  uint32_t bits = 0;
  read_bitstream(stream, 16, &bits, 1);
  uint32_t longueur = bits;
  bits = 0;
  int i = 0;
  char c;
  char *JFIF = "JFIF";
  while (i<5){
    read_bitstream(stream,8, &bits, 1);
    c = bits;
    bits = 0;
    if (JFIF[i] != c){
      break;
    }
    i++;
  }
  longueur -= 7;
  while (longueur != 0){
    read_bitstream(stream, 8, &bits, 1);
    longueur--;
  }
}

void parse_com(struct bitstream *stream){
  uint32_t bits = 0;
  read_bitstream(stream, 16, &bits, 1);
  uint longueur = bits - 2;
  bits = 0;

  while (longueur != 0){
    read_bitstream(stream, 8, &bits, 1);
    longueur--;
  }
}

void parse_dqt(struct bitstream *stream,uint8_t **tables, uint8_t *compteur){
  uint32_t bits = 0;
  read_bitstream(stream, 16, &bits, 1);
  uint longueur = bits - 2;
  bits = 0;

  uint8_t res[64];
  while (longueur != 0){
    read_bitstream(stream, 4, &bits, 1);
    uint8_t precision = bits;
    if (precision == 1){
      fprintf(stderr, "precision non gérée dans notre cas");
      exit(0);
    }
    bits = 0;
    read_bitstream(stream, 4, &bits, 1);
    uint32_t indice = bits;
    bits = 0;
    longueur--;

    int i = 0;
    while (i<64){
      read_bitstream(stream, 8, &bits, 1);
      res[i] = (uint8_t) bits;
      bits = 0;
      longueur --;
    }

    tables[indice] = res;
    *compteur+=1;
  } 
}

void parse_SOF0(struct bitstream *stream, struct donnees *donnees){
  uint32_t bits = 0;
  read_bitstream(stream, 16, &bits, 1);
  uint32_t longueur = bits - 2;
  bits = 0;

  read_bitstream(stream, 8, &bits, 1);
  donnees->precision = bits;
  bits = 0;
  longueur--;

  read_bitstream(stream, 16, &bits, 1);
  donnees->hauteur = bits;
  bits = 0;
  longueur-=2;

  read_bitstream(stream, 16, &bits, 1);
  donnees->largeur = bits;
  bits = 0;
  longueur-=2;

  read_bitstream(stream, 8, &bits, 1);
  donnees->nb_composantes = bits;
  bits = 0;
  longueur--;

  struct composante *tab_comp = malloc(donnees->nb_composantes*sizeof(struct composante));
  for (uint32_t i =0; i<donnees->nb_composantes; i++){
    struct composante comp;
    read_bitstream(stream, 8, &bits, 1);
    comp.identifiant = bits;
    bits = 0;
    longueur--;

    read_bitstream(stream, 4, &bits, 1);
    comp.facteur_h = bits;
    bits = 0;

    read_bitstream(stream, 4, &bits, 1);
    comp.facteur_v = bits;
    bits = 0;
    longueur--;

    read_bitstream(stream, 8, &bits, 1);
    comp.table_quant = bits;
    bits = 0;
    longueur--;

    comp.component = i;
    tab_comp[i] = comp;
  }
}

void parse_DHT(struct bitstream *stream, struct huff_table **table_AC, struct huff_table **table_DC,uint8_t *compteur){ 
  uint32_t bits = 0;
  read_bitstream(stream, 16, &bits, 1);
  uint32_t longueur = bits - 2;
  bits = 0;
  uint16_t bits_lus;
  while (longueur != 0){
    read_bitstream(stream, 3, &bits, 1);
    if (bits != 0){
      fprintf(stderr, "error reading huffman");
      exit(0);
    }

    read_bitstream(stream, 1, &bits, 1);
    uint8_t type = bits;
    bits =0; 
    read_bitstream(stream, 4, &bits, 1);
    uint8_t indice = bits;
    bits =0;
    longueur--;
    struct huff_table *table = load_huffman_table(stream, &bits_lus);
    if (type){
      table_AC[indice] = table;
    }
    else{
      table_DC[indice] = table;
    }
    longueur-=bits_lus;
    *compteur+=1;
  } 
}


void parse_SOS(struct bitstream *stream, struct bitstream **image, struct donnees *donnees){
  uint32_t bits = 0;
  read_bitstream(stream, 16, &bits, 1);
  uint32_t longueur = bits -2;
  bits = 0;
  
  read_bitstream(stream,8,&bits,1);
  if (bits != donnees->nb_composantes){
    fprintf(stderr, "wtf dude");
    exit(0);
  }
  bits = 0;
  longueur--;

  for (int i =0; i<donnees->nb_composantes;i++){
    read_bitstream(stream, 8, &bits,1);
    uint8_t id = bits;
    bits =0;
    longueur--;

    read_bitstream(stream, 4, &bits, 1);
    uint8_t id_dc = bits;
    bits =0;

    read_bitstream(stream, 4, &bits, 1);
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
  read_bitstream(stream, 24, &bits, 1);
  bits = 0;
  *image = stream;
}

struct jpeg_desc *read_jpeg(const char *filename){
  struct jpeg_desc *jpeg = malloc(sizeof(struct jpeg_desc));
  jpeg->filename = strdup(filename);
  struct bitstream *stream = create_bitstream(filename);
  uint16_t marqueur = 0;
  uint8_t flag=0;
  uint32_t bits = 0;
  uint8_t compteur_q;
  uint8_t compteur_h;
  uint8_t **tables = malloc(4*sizeof(uint8_t *));
  struct huff_table **table_AC = malloc(sizeof(struct huff_table *));
  struct huff_table **table_DC = malloc(sizeof(struct huff_table *));
  struct donnees *donnees = malloc(sizeof(struct donnees));
  struct bitstream **image = malloc(sizeof(struct bitstream *));
  
  read_bitstream(stream, 16, &bits, 1);
  if (bits != 0xffd8){
    fprintf(stderr,"whhallah c'est corrompu");
    exit(0);
  }

  while( !flag){
    read_bitstream(stream, 16, &bits, 1);
    marqueur = bits;
    bits = 0;
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
      parse_DHT(stream, table_AC, table_DC,&compteur_h);
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
    }
  }
  jpeg->nb_quant_table = compteur_q;
  jpeg->nb_huff_table = compteur_h;
  return jpeg;
}

struct bitstream *get_bitstream(const struct jpeg_desc *jpeg){
  return *(jpeg->image);
}

char *get_filename(const struct jpeg_desc *jpeg){
  return jpeg->filename;
}

uint8_t get_nb_quantization_tables ( const struct jpeg_desc *jpeg){
  return jpeg->nb_quant_table;
}

uint8_t get_nb_huff_tables(const struct jpeg_desc *jpeg){
  return jpeg->nb_huff_table;
}

uint8_t *get_quantization_table (const struct jpeg_desc *jpeg, uint8_t index){
  return jpeg->tables_quantification[index];
}

struct huff_table * get_huffman_table ( const struct jpeg_desc *jpeg ,enum acdc acdc ,uint8_t index ){
  if (acdc){
    return jpeg->table_AC[index];
  }
  else{
    return jpeg->table_DC[index];
  }
}

uint16_t get_image_size(struct jpeg_desc *jpeg, enum direction dir){
  if (dir){
    return jpeg->donnees->hauteur;
  }
  else{
    return jpeg->donnees->largeur;
  }
}

uint8_t get_nb_components ( const struct jpeg_desc *jpeg){
  return jpeg->donnees->nb_composantes;
}

uint8_t get_frame_component_id ( const struct jpeg_desc *jpeg ,uint8_t frame_comp_index ){
  return jpeg->donnees->composantes[frame_comp_index].identifiant;
}

uint8_t get_frame_component_sampling_factor(const struct jpeg_desc *jpeg, enum direction dir ,uint8_t frame_comp_index ){
  if (dir){
    return jpeg->donnees->composantes[frame_comp_index].facteur_h;
  }
  else{
    return jpeg->donnees->composantes[frame_comp_index].facteur_v;
  }
}

uint8_t get_frame_component_quant_index(const struct jpeg_desc *jpeg,uint8_t frame_comp_index ){
  return jpeg->donnees->composantes[frame_comp_index].table_quant;
}

