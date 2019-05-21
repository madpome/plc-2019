#include <stdint.h>
#include "huffman.h"
#include "bitstream.h"
#include "jpeg_reader.h"

extern void get_ind(struct bitstream *stream, uint32_t nb_bits, uint32_t *bits);
extern float trad_DC(struct bitstream *stream);
extern float trad_AC(struct bitstream *stream);
extern float *trad_block(struct bitstream *stream);

/// en gros pour le moment on s'occupe de la traduction du flux de bits en un tableau auquel il faudra appliquer la tf inverse

///get_ind intervient après avoir décodé un symbole de huffman (next_huffman_value)
///elle permet de connaitre l'indice dans la classe de magnetitude décodée par huffman
//elle lit donc nb_bits dans le stream, nb_bits est obtenu par huffman

///trad_DC traduit complètement un symbole DC
///elle traduit donc un symbole de huffman et trouve la valeur au bon indice dans la table de magnétude, elle peut également effectuer la trad du diff mais il faudra ajouter des paramètres

///trad_AC fait la même chose mais pour un symbole AC

///trad_block traduit entièrement un bloc (MCU)
///elle renvoie donc un tableau de flottant de taille 64 (8*8) tant qu'on fait des MCU de taille 8*8
