#include "../include/idct.h"
#include <math.h>
#include <stdio.h>
#define C(x) ((x==0)?(1/sqrt(2)):1)
#define PI 3.14159265359
#define SQRT_2_INV 0.70710678118
#define SQRT_2 1.41421356237
#define COS_6 0.38268343236
#define COS_1 0.9807852804
#define COS_3 0.8314696123
#define SIN_6 0.92387953251
#define SIN_3 0.55557023302
#define SIN_1 0.19509032201
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
            tab[i][j] /= 4;
            tab[i][j] +=128;
            tab[i][j] = (tab[i][j]<0)?0:tab[i][j];
            tab[i][j] = (tab[i][j]>255)?255:tab[i][j];
        }
    }
    return tab;
}
void transpose(float ** a){

    for(int i =0;i<8;i++){
        for(int j =i; j<8;j++){
            float tmp = a[i][j]/8 +128;
            a[i][j] = a[j][i]/8 +128;
            a[j][i] = tmp;
            a[i][j] = (a[i][j]<0)?0:a[i][j];
            a[i][j] = (a[i][j]>255)?255:a[i][j];
            a[j][i] = (a[j][i]<0)?0:a[j][i];
            a[j][i] = (a[j][i]>255)?255:a[j][i];
        }
    }
}
//Donne tous le bon résultat mais multiplié par 2,
//remettre au bon niveau avec un decalage a droite APRES avoir convertie en int
float **idct(int16_t **frequence){
    float **tab = malloc(8*sizeof(float*));
    for(int i = 0;i<8;i++){
        tab[i] = idct_ligne(NULL,frequence[i], 0);
    }
    float **tab_bis = malloc(8*sizeof(float*));
    for(int j = 0;j<8;j++){
        tab_bis[j] = idct_colonne(tab, j);
    }
    for(int i = 0;i<8;i++){
        free(tab[i]);
    }
    free(tab);
    transpose(tab_bis);
    return tab_bis;
}
float *idct_colonne(float **frequence, int col){
    float *I = malloc(8*sizeof(float));
    for(int j =0;j<8;j++){
        I[j]=frequence[j][col];
    }
    return idct_ligne(I,NULL, 1);
}
float *idct_ligne(float *frequence, int16_t *freq_true, int free_it){
    //algo loeffler modifié
    // http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.79.2772&rep=rep1&type=pdf
    float *I = malloc(8*sizeof(float));
    float *J = malloc(8*sizeof(float));
    /*
    I[0] = frequence[0];
    I[1] = frequence[4];
    I[2] = frequence[2];
    I[3] = frequence[6];
    I[4] = frequence[7];
    I[5] = frequence[3];
    I[6] = frequence[5];
    I[7] = frequence[1];
    */
    if(free_it == 1){
        I[0] = frequence[0];
        I[1] = frequence[1];
        I[2] = frequence[2];
        I[3] = frequence[3];
        I[4] = frequence[4];
        I[5] = frequence[5];
        I[6] = frequence[6];
        I[7] = frequence[7];
        free(frequence);
    }else{
        I[0] = (float)freq_true[0];
        I[1] = (float)freq_true[1];
        I[2] = (float)freq_true[2];
        I[3] = (float)freq_true[3];
        I[4] = (float)freq_true[4];
        I[5] = (float)freq_true[5];
        I[6] = (float)freq_true[6];
        I[7] = (float)freq_true[7];
    }
    //Etape 1
    J[0] = I[0] + I[4];
    J[1] = I[0] - I[4];
    J[2] = I[2]*SQRT_2*COS_6-I[6]*SQRT_2*SIN_6;
    J[3] = I[2]*SQRT_2*SIN_6+I[6]*SQRT_2*COS_6;
    J[4] = SQRT_2_INV*I[7];
    J[5] = I[3];
    J[6] = I[5];
    J[7] = SQRT_2_INV*I[1];
    //Etape 2
    I[0] = J[0]+J[3];
    I[1] = J[1]+J[2];
    I[2] = J[1]-J[2];
    I[3] = J[0]-J[3];
    I[4] = J[7]-J[4];
    I[7] = J[7]+J[4];
    //Etape 2.5
    I[5] = I[7]-J[5];
    I[6] = I[4]-J[6];
    I[7] = I[7] + J[5];
    I[4] = I[4] + J[6];
    //Etape 3
    J[5] = I[5]*SQRT_2*COS_1-I[6]*SQRT_2*SIN_1;
    J[6] = I[5]*SQRT_2*SIN_1+I[6]*SQRT_2*COS_1;
    J[4] = I[4]*SQRT_2*COS_3-I[7]*SQRT_2*SIN_3;
    J[7] = I[4]*SQRT_2*SIN_3+I[7]*SQRT_2*COS_3;
    //Etape 4
    J[0] = I[0] + J[7];
    J[1] = I[1] + J[6];
    J[2] = I[2] + J[5];
    J[3] = I[3] + J[4];
    I[4] = I[3]-J[4];
    I[5] = I[2]-J[5];
    I[6] = I[1]-J[6];
    I[7] = I[0]-J[7];
    I[0] = J[0];
    I[1] = J[1];
    I[2] = J[2];
    I[3] = J[3];
    free(J);
    return I;
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
/*
int main(int argc, char **argv){
    int16_t **test = malloc(8*sizeof(int16_t*));
    for(int i =0;i<8;i++){
        test[i] = malloc(8*sizeof(int16_t));
    }
    float **res;
    int a = 0;
    for(int i =0;i<8;i++){
        for(int j =0; j<8;j++){
            test[i][j] = a++;
        }
    }

    for(int i =0;i<8;i++){
        for(int j =0; j<8;j++){
            printf("%d ",test[i][j]);
        }
        printf("\n");
    }
    printf("\n");

    res = naive_idct(test, cos_table());
    for(int i =0;i<8;i++){
        for(int j =0; j<8;j++){
            printf("%f ",res[i][j]);
        }
        printf("\n");
    }
    printf("\n\n\n\n");
    printf("\n");
    res = idct(test);
    for(int i =0;i<8;i++){
        for(int j =0; j<8;j++){
            printf("%f ",res[i][j]/2);
        }
        printf("\n");
    }
    return 0;
}
*/
