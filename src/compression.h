#include <stdint.h>
/* Prend en entré 8 bits et renvoie l'entier associé aux 4 bits de poids fort*/
public uint_8 read_high(uint_8 symbol){
    return symbol & 240;
}
/*Idem que read_high mais avec les bits de poids faible*/
public uint_8 read_low(uint_8 symbol){
    return symbo & 15;
}
