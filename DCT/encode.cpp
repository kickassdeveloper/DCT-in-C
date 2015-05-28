/*
	encode.cpp
	
	>>Konvertuje vrednosti RGB u YCbCr i cuvaka konverotvani sadrzaj.
	
	>>Kompajliranje: g++ -c encode.cpp
*/
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "rgb_ycc.h"
#include "dct.h"

void print_dct_block ( int Y[] )
{
	for ( int i = 0; i < 64; ++i )
	{
		if ( i % 8 == 0 ) printf("\n");
			printf("%d,\t", Y[i] );
	}
	printf("\n-----------------");
}

void save_one_dctblock ( int Y[], FILE *fpo )
{
	short Ys[64];
	for ( int i = 0; i < 64; ++i ) //[pretvoriti u short zbog cuvanja
		Ys[i] = ( short ) Y[i];
	fwrite ( Ys, 2, 64, fpo ); //cuva DCT koeficijent bloka
}
/*
* Vrsi DCT na sest 8x8 uzorak blokova 4:2:0 YCbCr makrobloka
* cuva koeficijente u fp0
*/
void save_dct_yccblocks( YCbCr_MACRO *ycbcr_macro, FILE *fpo )
{
	short block, i, j, k;
	unsigned char *py;
	int X[64], Y[64];//za dct transformaciju
	//cuva DCT od Y
	for ( block = 0; block < 4; block++)
	{//Y ima 4 8x8 uzorak bloka
		if ( block < 2 )
			py=(unsigned char *)&ycbcr_macro->Y+8*block;//pocetak bloka
		else //pokazuje na  pocetak bloka
			py = (unsigned char *)&ycbcr_macro->Y+128+8*(block-2);
		k = 0;
		for ( i = 0; i < 8; i++ ) //jedna uzorak bloka
		{
			if ( i > 0 ) 
				py += 16; //uvecaj i za 16
			for ( j = 0; j < 8; j++ ) 
			{
			X[k++] = ( int ) *( py + j );
			}
		}
		dct ( X, Y );//DCT transformacija od 8x8 bloka
		save_one_dctblock ( Y, fpo ); //cuva DCT koeficijente  od 8x8 bloka
	}
	k = 0;
	for ( i = 0; i < 8; ++i ) 
	{
		for ( j = 0; j < 8; ++j ) 
		{
			X[k] = ycbcr_macro->Cb[k];
			k++;
		}
	}	
	dct ( X, Y ); //DCT od Cb 8x8 uzorak bloka
	save_one_dctblock( Y, fpo );
	k = 0;

	for ( i = 0; i < 8; ++i ) 
	{
		for ( j = 0; j < 8; ++j )
		{
			X[k] = ycbcr_macro->Cr[k];
			k++;
		}
	}
	dct ( X, Y );//DCT od Cr 8x8 uzorak bloka
	save_one_dctblock ( Y, fpo );
}
/*
* Konvertuje RGB vrednosti u YCbCr, pa u DCT koeficijente i
* cuva DCT koeficijente.
*/
void encode_dct ( RGBImage *image, FILE *fpo )
{
	short row, col, i, j, r;
	RGB *p;//pointer to a pixel
	RGB macro16x16[256];//16x16 macroblock;24-bit RGB pixel
	YCbCr_MACRO ycbcr_macro;//macroblock for YCbCr samples
	static int nframe = 0;
	
	for ( row = 0; row < image->height; row += 16 ) 
	{
		for ( col = 0; col < image->width; col += 16 ) 
		{
			//pokazivac na pocetak makrobloka
			p = (RGB *)image->ibuf+(row * image->width + col);
			r = 0;
			for ( i = 0; i < 16; ++i ) 
			{
				for ( j = 0; j < 16; ++j ) 
				{
				macro16x16[r++] = (RGB) *p++;
				}
			p += (image->width-16); //sledeci red u makrobloku
			}
			macroblock2ycbcr ( macro16x16, &ycbcr_macro );
			save_dct_yccblocks( &ycbcr_macro, fpo );
		} //za col
	} //za row
}

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

