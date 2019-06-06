#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "../include/huffman.h"

enum component {
  COMP_Y ,
  COMP_Cb ,
  COMP_Cr ,
  /* sentinelle */
  COMP_NB
};
enum direction {
  DIR_H ,
  DIR_V ,
  /* sentinelle */
  DIR_NB
};
enum acdc {
  DC = 0,
  AC = 1,
  /* sentinelle */
  ACDC_NB
};

struct composante{
  uint8_t identifiant;
  enum component component;
  uint8_t facteur_h;
  uint8_t facteur_v;
  uint8_t table_quant;
  uint8_t ind_huffman_dc;
  uint8_t ind_huffman_ac;
};

struct donnees{
  uint8_t precision;
  uint16_t hauteur;
  uint16_t largeur;
  uint8_t nb_composantes;
  struct composante *composantes;
};

struct jpeg_desc{
  uint8_t nb_quant_table;
  uint8_t **tables_quantification;
  struct donnees *donnees;
  uint8_t nb_huff_table;
  struct huff_table **table_AC;
  struct huff_table **table_DC;
  struct bitstream **image;
  char *filename;
};

extern struct jpeg_desc *read_jpeg(const char *filename);

extern struct bitstream *get_bitstream(const struct jpeg_desc *jpeg);

extern char *get_filename(const struct jpeg_desc *jpeg);

extern uint8_t get_nb_quantization_tables ( const struct jpeg_desc *jpeg);

extern uint8_t get_nb_huff_tables(const struct jpeg_desc *jpeg);

extern uint8_t * get_quantization_table ( const struct jpeg_desc *jpeg ,
uint8_t index );

extern struct huff_table * get_huffman_table ( const struct jpeg_desc *jpeg ,enum acdc acdc ,uint8_t index );

extern uint16_t get_image_size ( struct jpeg_desc *jpeg ,enum direction dir);

extern uint8_t get_nb_components ( const struct jpeg_desc *jpeg);

extern uint8_t get_frame_component_id ( const struct jpeg_desc *jpeg ,uint8_t frame_comp_index );

extern uint8_t get_frame_component_sampling_factor(const struct jpeg_desc *jpeg ,enum direction dir,uint8_t frame_comp_index );

extern uint8_t get_frame_component_quant_index(const struct jpeg_desc *jpeg ,uint8_t frame_comp_index );

void parse_SOS(struct bitstream *stream, struct bitstream **image, struct donnees *donnees);

void parse_APP0(struct bitstream *stream);

void parse_com(struct bitstream *stream);

void parse_dqt(struct bitstream *stream,uint8_t **tables, uint8_t *compteur);

void parse_DHT(struct bitstream *stream, struct huff_table **table_AC, struct huff_table **table_DC,uint8_t *compteur);

void parse_SOF0(struct bitstream *stream, struct donnees *donnees);

