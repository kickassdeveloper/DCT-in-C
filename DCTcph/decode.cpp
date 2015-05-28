/*
	decode.cpp
 
	>>Sadrzi funkcije koje citaju YCbCr vrednosti iz fajla i pretvaraju ih u  RGB.
	>>Kompajlira: g++ -c decode.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include "rgb_ycc.h"

/*
	Uzima  YCbCr vrednosti na koje pokazuje fpi. Uzima cetri 8x8 Y uzorak
	bloka, jedan 8x8 Cb uzorak blok i jedan 8x8 Cr uzirak blok i stavlja ih u strukturu
	YCbCr_MACRO.

	Vraca: vraca broj bajtova iz datoteke i smesta ih YCbCr_MACRO strukturu.
*/
int get_yccblocks( YCbCr_MACRO *ycbcr_macro, FILE *fpi )
{
	short r, row, col, i, j, k, n, block;
	short c;
	unsigned char *py;
	n = 0;
	//cita vrednosti iz cetri 8x8 Y uzorak bloka
	for ( block = 0; block < 4; block++ )
	{
		if ( block < 2 ) //points to beginning of block
			py = ( unsigned char * ) &ycbcr_macro->Y + 8*block;
		else
			py = (unsigned char *)&ycbcr_macro->Y+128+8*(block-2);
		
		for ( i = 0; i < 8; i++ ) 
		{
		//jedan uzorak-blok
			if ( i > 0 ) 
				py += 16; //uvecavamo za 16 (jedan red)
			for ( j = 0; j < 8; j++ ) 
			{
				if ( ( c = getc ( fpi )) == EOF) //proverava da li je kraj
					break;
				*(py + j) = (unsigned char) c; //cuva u strukturu YCbCr_MACRO
				n++;	
			}
		}
	} //za blokove

	//za 8x8 Cb blok
	k = 0;
	for ( i = 0; i < 8; ++i )
	{
		for ( j = 0; j < 8; ++j ) 
		{
			if ( ( c = getc ( fpi )) == EOF )
				break;
			ycbcr_macro->Cb[k++] = (unsigned char )c;
			n++;
		}
	}

	//now do that for 8x8 Cr block
	k = 0;
	for ( i = 0; i < 8; ++i )
	{
		for ( j = 0; j < 8; ++j ) 
		{
			if ( ( c = getc ( fpi )) == EOF )
				break;
			ycbcr_macro->Cr[k++] = (unsigned char) c;
			n++;
		}
	}
return n;
//broj bajtova procitan
}
/*
Konvertovanje YCbCr oblika u RGB oblik.
*
*/
int decode_yccFrame ( RGBImage &image, FILE *fpi )
{
	short r, row, col, i, j, k, block;
	int n = 0;
	RGB macro16x16[256];	//16x16 pixel makroblok; 24-bita RGB pixel
	YCbCr_MACRO ycbcr_macro;//makroblok za YCbCr uzorke
	RGB *rgbp; //referenca na RGB pixel
	
	for ( row = 0; row < image.height; row += 16 ) 
	{
			for ( col = 0; col < image.width; col += 16 )
			{
				int m = get_yccblocks( &ycbcr_macro, fpi );
				if ( m <= 0 ) 
				{
					printf("\nnema podataka\n");
					return m;
				}
				n += m;
				ycbcr2macroblock( &ycbcr_macro, macro16x16 );
				rgbp = ( RGB *)(image.ibuf)+(row*image.width+col);//referenca na pocetak makrobloka
				r = 0;
				
				for ( i = 0; i < 16; ++i ) 
				{
					for ( j = 0; j < 16; ++j ) 
					{
						*rgbp++ = macro16x16[r++];
					}
					rgbp += (image.width - 16);//sledeci red u  makrobloku
				}
			} //za redove
	} //za kolone
	return n;
}
