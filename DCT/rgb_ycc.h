void rgb2ycbcr( RGB &rgb, YCbCr &ycc );
void rgb2y( RGB &rgb, unsigned char &y );
void chop ( int & r, int &g, int &b );
void ycbcr2rgb(YCbCr &ycc, RGB &rgb);
void macroblock2ycbcr ( RGB *macro16x16, YCbCr_MACRO *ycbcr_macro );
void ycbcr2macroblock( YCbCr_MACRO *ycbcr_macro, RGB *macro16x16 );

