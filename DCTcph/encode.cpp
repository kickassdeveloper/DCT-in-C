/*
	encode.cpp
	
	>>Konvertuje vrednosti RGB u YCbCr i cuvaka konverotvani sadrzaj.
	
	>>Kompajliranje: g++ -c encode.cpp
*/
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "rgb_ycc.h"

//cuva jedan YCbCr makroblok.
void save_yccblocks( YCbCr_MACRO *ycbcr_macro, FILE *fpo )
{
	short block, i, j, k;
	unsigned char *py;
	
	//cuva cetri 8x8 Y uzorak blokova
	for ( block = 0; block < 4; block++ ) 
	{
		if ( block < 2 )//pokazuje na pocetak bloka
			py = ( unsigned char * ) &ycbcr_macro->Y + 8*block;
		else
			py =(unsigned char *)&ycbcr_macro->Y+128+8*(block-2);

		for ( i = 0; i < 8; i++ ) //jedan uzorka blok
		{ 
			if ( i > 0 ) 
				py += 16;	//napreduje py za 16
			
			for ( j = 0; j < 8; j++ ) 
			{
			putc ( ( int ) *(py+j),fpo);//cuva jedan bajt 
			}
		}
	}

	//cuva jedan 8x8 Cb blok
	k = 0;
	for ( i = 0; i < 8; ++i ) 
	{
		for ( j = 0; j < 8; ++j ) 
		{
			putc( ( int ) ycbcr_macro->Cb[k++], fpo );
		}
	}
	//cuva jedan 8x8 Cr blok
	k = 0;
	for ( i = 0; i < 8; ++i ) 
	{
		for ( j = 0; j < 8; ++j ) 
		{
			putc( ( int ) ycbcr_macro->Cr[k++], fpo );
		}
	}
}

/*
	Konvertuje RGB to YCbCr i cuva konvertovane  vrednosti.
*/
void encode( RGBImage *image, FILE *fpo )
{
	short row, col, i, j, r;
	
	RGB macro16x16[256];	//16x16 pixela makroblok;24-bit RGB pixel
	YCbCr_MACRO ycbcr_macro;//mackroblokovi za YCbCr uzorke
	RGB *p;//referenca na RGB pixel

	static int nframe = 0;
	
	for ( row = 0; row < image->height; row += 16 ) 
	{
		for ( col = 0; col < image->width; col += 16 ) 
		{
			p=(RGB *)image->ibuf+(row*image->width + col); //referenca na pocetak makrobloka
		r = 0;
		for ( i = 0; i < 16; ++i ) 
		{
			for ( j = 0; j < 16; ++j ) 
			{
				macro16x16[r++] = (RGB) *p++;
			}
			p += (image->width-16); //points to next row within macroblock
		}
		macroblock2ycbcr ( macro16x16, &ycbcr_macro );//RGB u YCbCr
		save_yccblocks( &ycbcr_macro, fpo ); //save one YCbCr macroblock
		} //za col
	} //za row
}

