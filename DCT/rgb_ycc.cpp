#include "common.h"


/* 
   >>konvertuje iz RGB u YCbCr koristeci ITU-R preporuku
   	Y = 0.299R + 0.587G + 0.114B
	Cb = 0.564(B - Y ) + 0.5
	Cr = 0.713(R - Y ) + 0.5

   >>koristimo inidzer aritmetiku da ubrzamo racunanje
 	Napomena:
			2^16=65536
			kr= 0.299 = 19595 / 2^16
			kg = 0.587 = 38470 / 2ˆ16
			kb = 0.114 = 7471 / 2ˆ16
			0.5 = 128 / 255
			0.564 = 36962 / 2ˆ16

   >>Ulaz: RGB pixel
   >>Izlaz: YCbCr pixel

 */


void rgb2ycbcr( RGB &rgb, YCbCr &ycc )
{
	//koeficijenti se sumiraju 65536(1<<16), tako da Y ostaje u intervalu [0, 255]
	ycc.Y = (unsigned char)((19595 * rgb.R + 38470 * rgb.G + 7471 * rgb.B ) >> 16);
	ycc.Cb = (unsigned char)((36962*(rgb.B - ycc.Y ) >> 16) + 128);
	ycc.Cr = (unsigned char)((46727*(rgb.R - ycc.Y ) >> 16) + 128);
}
//konverzija RGB pixela samo u Y vrednost
void rgb2y( RGB &rgb, unsigned char &y )
{
y = (short)((19595*rgb.R + 38470*rgb.G + 7471*rgb.B ) >> 16);
}

//funkcija provere opseka boja
void chop ( int & r, int &g, int &b )
{
	if ( r < 0 ) r = 0;
	else if ( r > 255 ) r = 255;
	if ( g < 0 ) g = 0;
	else if ( g > 255 ) g = 255;
	if ( b < 0 ) b = 0;
	else if ( b > 255 ) b = 255;
}

/*
		Konvertovanje iz YCbCr u RGB oblik, koristeci ITU-R standard
			R = Y + 1.402Cr- 0.701
			G = Y - 0.714Cr- 0.344Cb + 0.529
			B = Y + 1.772Cb- 0.886

*/

void ycbcr2rgb(YCbCr &ycc, RGB &rgb)
{
	int r,g,b;

	r = (ycc.Y + (91881 * ycc.Cr >> 16) - 179);
	g = (ycc.Y - ((22544 * ycc.Cb + 46793 * ycc.Cr) >> 16) + 135);
	b = (ycc.Y +  (116129 * ycc.Cb >> 16) -226);

	chop(r,g,b); //ogranicenje opseka
	rgb.R	= (unsigned char) r;
	rgb.G	= (unsigned char) g;
	rgb.B 	= (unsigned char) b;
}
/*
		Konvertovanje  RGB makrobloka ( 16x16 ) u 4:2:0 YCbCr uzorak_blokove ( sest 8x8 blokova ).
*/
void macroblock2ycbcr ( RGB *macro16x16, YCbCr_MACRO *ycbcr_macro )
{
	int i, j, k, r;
	
	YCbCr ycb;
	
	for ( i = 0; i < 16; ++i ) 
	{
		for ( j = 0; j < 16; ++j ) 
		{
			if (!( i & 1 ) && !( j & 1 ))//jedan Cb, Cr  za svaka 4 pixela
			{					
				rgb2ycbcr(macro16x16[r], ycb);//convert to Y, Cb, Cr values
				ycbcr_macro->Y[r] = ycb.Y;
				ycbcr_macro->Cb[k] = ycb.Cb;
				ycbcr_macro->Cr[k] = ycb.Cr;
				k++;
			} 
			else 
			{
				rgb2y ( macro16x16[r], ycbcr_macro->Y[r] );//potrebna nam je samo Y komponenta za preostal 3 pixela
			}
			r++;
			//konvertujemo svaki pikzel za Y
		}
	}
}

/*
Konvertuje sest 8x8 YCbCr uzorak blokova u RGB makroblokove(16x16).
*/
void ycbcr2macroblock( YCbCr_MACRO *ycbcr_macro, RGB *macro16x16 )
{
	int i, j, k, r;
	short y;
	YCbCr ycb;
	r = k = 0;
	for ( i = 0; i < 16; ++i ) 
	{
		for ( j = 0; j < 16; ++j )
		{
			//jedan Cb, Cr je sacuvan za svaka 4 pixela
			if ( !( i & 1 ) && !( j & 1 ) )
			{
				ycb.Y = ycbcr_macro->Y[r];
				ycb.Cb = ycbcr_macro->Cb[k];
				ycb.Cr = ycbcr_macro->Cr[k];
				ycbcr2rgb ( ycb, macro16x16[r]);
				k++;
			} 
			else 
			{
				ycb.Y = ycbcr_macro->Y[r];
				ycbcr2rgb( ycb, macro16x16[r] );
			}
			r++;
		}
	}
}


