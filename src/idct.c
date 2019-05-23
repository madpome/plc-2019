#include "../include/idct.h" 
#define C(x) ((x==0)?(1/sqrt(2)):1)
// A COMPILER AVEC -lm
float ** naive_idct(int32_t **frequence, float ** table_cos){
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

int main(int argc, char ** argv){
    float **table_cos = malloc(8*sizeof(float*));
    for(int i =0;i<8;i++){
        table_cos[i] = malloc(8*sizeof(float));
    }
    for(int i =0;i<8;i++){
        for(int j = 0;j<8;j++){
            //table_cos[i][j] = cos((2i+1)*j*pi/16)
            table_cos[i][j] = cos((2*i+1)*j*M_PI/16);
        }
    }
}
