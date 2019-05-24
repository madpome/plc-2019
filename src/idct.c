#include "../include/idct.h"
#include <math.h>
#define C(x) ((x==0)?(1/sqrt(2)):1)
#define PI 3.14159265359
// A COMPILER AVEC -lm
float ** naive_idct(int16_t **frequence, float ** table_cos){
    float **tab = malloc(8*sizeof(float*));
    for(int i =0;i<8;i++){
        tab[i] = malloc(8*sizeof(float));
    }
    for(int i =0;i<8;i++){
        for(int j = 0;j<8;j++){
            for(int lambda =0;lambda<8;lambda++){
                for(int gamma = 0;gamma<8;gamma++){
                    tab[i][j]+=C(lambda)*C(gamma)*table_cos[i][lambda]*table_cos[j][gamma]*frequence[lambda][gamma];
                }
            }
        }
    }
    return tab;
}

float **cos_table(){
    float **table_cos = malloc(8*sizeof(float*));
    for(int i =0;i<8;i++){
        table_cos[i] = malloc(8*sizeof(float));
    }
    for(int i =0;i<8;i++){
        for(int j = 0;j<8;j++){
            //table_cos[i][j] = cos((2i+1)*j*pi/16)
            table_cos[i][j] = cos((2*i+1)*j*PI/16);
        }
    }
    return table_cos;
}
