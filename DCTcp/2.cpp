#include <Magick++.h> 
#include <iostream> 
#include <string>
using namespace std; 
using namespace Magick; 
int main(int argc,char **argv) 
{ 
  InitializeMagick(*argv);//inicijalizacija magick++
  string s=argv[2]; //-ekstenzija za cuvanje
  string format= argv[1];//naziv slike, sluzi za ekstraktovanje formata
  int du=format.length();
  
  //provera da li je nas foramt
  if(argv[1][du-4]=='r' && argv[1][du-3]=='s' && argv[1][du-2]=='1' && argv[1][du-1]=='4')
  {
  //vrsimo dekompresiju
	  cout<< "OVO RADI" <<endl;
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
    int h_visak=h%8;
    int w_visak=h%8;


    if(h_visak!=0 || w_visak!=0)
    {
	    //funkcija prima vrednost (sirina,visina, xofest,yofest)
	    int razH=h-h_visak;
	    int razW=w-w_visak;
	    image.crop( Geometry(razW,razH));
    }

    if(s=="rs14")
    {
    	//vrsimo komresiju
    }
    else
    {
    // pisanje nove slike sa kompresijom
    cout<< "OVO RADI" <<endl;
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
