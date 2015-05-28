#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "decode.h"
#include "encode.h"

class CImage {
	public:
		unsigned char red;
		unsigned char green;
		unsigned char blue;
};

//void encode ( RGBImage *image, FILE *fpo );
//int decode_yccFrame ( RGBImage &image, FILE *fpi );
/*
* Pravi PPM header od sirine i duzine slike. "P6" format se koristi.
* PPM  header vraca nizi intidzer u  ppmh[].
*/
void make_ppm_header ( int ppmh[], int width, int height )
{
	//standardne header vrednosdti, ’P’ = 0x50, ’6’ = 0x36, ’\n’ = 0x0A
	int ca[] = 
	{	0x50, 0x36, 0x0A,			//"P6"
		0x33, 0x36, 0x30, 0x20,			//image sirina=260,duzina=288
		0x32, 0x38, 0x38, 0x0A,
		0x32, 0x35, 0x35, 0x0A			//nivo boja=256
	}; 

	char temp[10], k;
	sprintf(temp, "%3d", width );	//sirina u ascii kodu
	k = 0;
	for ( int i = 3; i <= 5; ++i )//zamena sirine
		ca[i] = temp[k++];

	sprintf(temp, "%3d", height );//visina u ascii kodu
	k = 0;
	for ( int i = 7; i <=9; ++i )//zamena visine
		ca[i] = temp[k++];

	for ( int i = 0; i < 15; ++i )
		ppmh[i] = ca[i]; //formiramo heder
}

void save_ppmdata ( RGBImage &image, FILE *fp )
{
	int size = 3 * image.width * image.height;
	for ( int i = 0; i < size; ++i )
	putc ( image.ibuf[i], fp ) ;
}

void ppm_read_comments ( FILE *fp )
{
	int c;
	while ( (c = getc ( fp ) ) == ’#’ )
	{
		while ( getc( fp ) != ’\n’ );
	}
	ungetc ( c, fp );
}

class ppm_error
{
	public:
	ppm_error() 
	{
	printf("\nNepravilan PPM format!\n");
	exit ( 1 );
	}
};

void write_ycc_header( short width, short height, FILE *fpo )
{
	char header[] = { ’Y’, ’C’, ’b’, ’C’, ’r’, ’4’, ’2’, ’0’ };
	for ( int i = 0; i < 8; ++i )
		putc ( header[i], fpo );
	char *p;
	p = ( char *) &width; //pokazuje na manji bajt sirine
	putc ( *p++, fpo ); //cuva manji bajt sirine
	putc ( *p, fpo ); //cuva visi bajt sirine
	p = ( char *) &height;//pokazuje na manji bajt visine
	putc ( *p++, fpo );//cuva manji bajt visine
	putc ( *p, fpo );//cuva visi bajt visine
}

int read_ycc_header( short &width, short &height, FILE *fpi )
{
	char header[9];
	for ( int i = 0; i < 8; ++i )
		header[i] = (char) getc ( fpi );
	if ( strncmp ( header, "YCbCr420", 8 ) )
		return -1;
	char *p;
	p = ( char *) &width;//cita sirinu
	*p++ = getc ( fpi );
	*p = getc ( fpi );
	p = ( char *) &height;//cita visinu
	*p++ = getc ( fpi );
	*p = getc ( fpi );
	return 1;
}

class ppm_error
{
	public:
	ppm_error() 
	{
	printf("\nIncorrect PPM format!\n");
	exit ( 1 );
	}
};

