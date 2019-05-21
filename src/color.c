
typedef struct RGB{
    //Si l'image est en noir et blanc, on met le niveau de gris dans la composante R
    uint8_t R;
    uint8_t G;
    uint8_t B;
}RGB;


public RGB ycbcr_to_rgb(float Y, float C_r, float C_b){
    RGB rgb;
    float a = Y + 1.402*(C_r-128);
    a = (r<0)?0:a;
    a = (r>255)?255:a;
    rgb.R = (uint_8t) a;
    a = Y - 0.34414*(C_b - 128) - 0.71313*(C_r - 128);
    a = (r<0)?0:a;
    a = (r>255)?255:a;
    rgb.G = (uint_8t) a;
    a = Y + 1.772*(C_b - 128);
    a = (r<0)?0:a;
    a = (r>255)?255:a;
    rgb.B = (uint8_t ) a;
}
