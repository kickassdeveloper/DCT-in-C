#include <Magick++.h> 
#include <iostream> 
#include <string>
#include "common.h"
#include "dct.h"
#include "encode.h"
#include "decode.h"
using namespace std; 
using namespace Magick; 


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
	while ( (c = getc ( fp ) ) == ’#’ ){
		while ( getc( fp ) != ’\n’ )
            ;
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
int main(int argc,char **argv) 
{ 
  InitializeMagick(*argv);//inicijalizacija magick++
  string s=argv[2]; //-ekstenzija za cuvanje
  string format= argv[1];//naziv slike, sluzi za ekstraktovanje formata
  int du=format.length();
  FILE *fp;
  int c;
  int isize;

  
  //provera da li je nas foramt
  if(argv[1][du-3]=='d' && argv[1][du-2]=='c' && argv[1][du-1]=='t')
  {
 	 fp = fopen ( "image.dct", "rb" );
	if ( read_dct_header ( image.width, image.height, fp ) == -1 )
		{
		printf("\nNije DCT fajl\n");
		return 1;
		}
	isize = image.width * image.height;
	image.ibuf = ( unsigned char *) malloc ( 3 * isize );
	decode_dct ( image, fp );//izvrsava se IDCT, convert konvertuje se RGB
	fclose ( fp );
	//suva se
	fp = fopen ("image.ppm", "wb");
	//izlazni  PPM fajl
	int ppmh[20];
	//PPM header*
	make_ppm_header ( ppmh, image.width, image.height );
	for ( int i = 0; i < 15; ++i )//cuva PPM header
		putc ( ppmh[i], fp );
	save_ppmdata ( image, fp );//cuva RGB vrednosti
	printf("\nVracena vrednost smestena u image.ppm\n");
	fclose ( fp );
	delete image.ibuf;//dealocira  memoriju
	return 0;
  }
  else
  {
  Image image;
  try {


    image.read( argv[1] );
    //uzimanje sirine i visine slike;
    int h = image.rows();
    int w = image.columns();
    //posto DCT radi sa matricom 8x8
    //provera da li je dimnenzija unete slike podobna
    //ako nije odesca se ostatak
    int h_visak=h%16;
    int w_visak=h%16;


    if(h_visak!=0 || w_visak!=0)
    {
	    //funkcija prima vrednost (sirina,visina, xofest,yofest)
	    int razH=h-h_visak;
	    int razW=w-w_visak;
	    image.crop( Geometry(razW,razH));
    }

    if(s=="ppm")
    {
	fp = fopen ("image.ppm", "rb");//PPM 
	RGBImage image;
	ppm_read_comments ( fp );
	char temp[100];
	//read comments
	fscanf ( fp, "%2s", temp );
	temp[3] = 0;
	if ( strncmp ( temp, "P6", 2 ) )
		throw ppm_error();
	ppm_read_comments ( fp );
	fscanf ( fp, "%d", &image.width );
	ppm_read_comments ( fp );
	fscanf ( fp, "%d", &image.height );
	ppm_read_comments ( fp );
	int colorlevels;
	fscanf ( fp, "%d", &colorlevels );
	ppm_read_comments ( fp );

	while ((c = getc ( fp )) == ’\n’);
	ungetc ( c ,fp );
	
	//konvertuje se RGB vrednost u  YCbCr 4:2:0 a posle ud DCTi cuva se
	// cuva se  "image.dct"
	
	isize = image.width * image.height;
	//alokacija memorije za RGB vrednosti
	image.ibuf = ( unsigned char *) malloc (3 * isize);
	fread ( image.ibuf, 3, isize, fp );
	fclose ( fp );
	fp = fopen ( "image.dct", "wb" );
	write_dct_header ( image.width, image.height, fp );
	encode_dct ( &image, fp );
	
	//pretvara 16-bit DCT  koeficijente
	//sacuvana vrednost u  "image.dct"
	
	delete image.ibuf;
	//brise se bafer
	fclose ( fp );    	
    }
    else
    {
    // pisanje nove slike sa kompresijom
    image.write( "x"+s );
    }
  } 
  catch( Exception &error_ ) 
    { 
      cout << "Caught exception: " << error_.what() << endl; 
      return 1; 
    } 
  }
  return 0; 
}
