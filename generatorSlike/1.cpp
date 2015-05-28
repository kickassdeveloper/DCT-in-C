#include <Magick++.h>
#include <iostream>
#include <stdlib.h>

using namespace std; 
using namespace Magick; 
int main(int argc,char **argv) 
{ 
  InitializeMagick(*argv);
  int r=rand()%255;
  int g=rand()%255;
  int b=rand()%255;
  int w=atoi(argv[1]);
  int h=atoi(argv[2]);
  string ext=argv[3];

  Image image( Geometry(w, h), "white" );
  srand (time(NULL));
  int i,j;
  for(i=0;i<w;i++)
  {
      for(j=0;j<h-2;j+=3)
      {
	  int r=rand()%65535;
	  int g=rand()%65535;
	  int b=rand()%65535;
	  int a=rand()%65535;
          image.pixelColor( i, j, Color(r,g,b,a));
          image.pixelColor( i, j+1, Color (r,g,b,a));
          image.pixelColor( i, j+2, Color (r,g,b,a));

      }
  }
  image.write( "random"+ext);
}
