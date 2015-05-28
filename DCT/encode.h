void print_dct_block ( int Y[]);
void save_one_dctblock ( int Y[], FILE *fpo );
void save_dct_yccblocks( YCbCr_MACRO *ycbcr_macro, FILE *fpo );
void encode_dct ( RGBImage *image, FILE *fpo );
void save_yccblocks( YCbCr_MACRO *ycbcr_macro, FILE *fpo );
void encode( RGBImage *image, FILE *fpo );

