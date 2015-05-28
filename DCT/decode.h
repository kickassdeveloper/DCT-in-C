int get_one_dctblock ( int Y[], FILE *fpi );
int get_dct_yccblocks( YCbCr_MACRO *ycbcr_macro, FILE *fpi );
int decode_dct ( RGBImage &image, FILE *fpi );
int get_yccblocks( YCbCr_MACRO *ycbcr_macro, FILE *fpi );
int decode_yccFrame ( RGBImage &image, FILE *fpi );


