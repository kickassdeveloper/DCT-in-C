#include <string.h>
#include <stdio.h>
#include <math.h>

#define PI 3.141592653589
const short shift = 10; //10 bits preciznost

// u korajjem stadijumu vrednosti su siftovane dva buta
const short shift1 = 2 * shift;
const int fac = 1 << shift ; //pomnoziti sve konstatnte sa 2ˆ10
const int delta = 1 << (shift-1);//za zaokruzivanje ̃0.5x2ˆ10
const int delta1 = 1 << (shift1-1);//za konacno zaokruzivanje ̃0.5x2ˆ20
const double a = PI / 16.0; //ugao teta

//DCT konstante koristimoa intidzer aritmetiku
const int c0 = (int)(1 / sqrt ( 2 ) * fac );
const int c1 = (int)( cos ( a ) / 2 * fac );
const int c2 = (int)( cos ( 2*a ) / 2 * fac );
const int c3 = (int)( cos ( 3*a ) / 2 * fac );
const int c4 = (int)( cos ( 4*a ) / 2 * fac );
const int c5 = (int)( cos ( 5*a ) / 2 * fac );
const int c6 = (int)( cos ( 6*a ) / 2 * fac );
const int c7 = (int)( cos ( 7*a ) / 2 * fac );

/*
DCT funkcija
	>>Ulaz: X, niz 8x8, koji sadrzi vrednosti [0,255]
	>>Izlaz: Y, niz 8x8 koji sadrzi DCT koeficijente
*/

void dct(int *X, int *Y)
{
	int i, j, j1, k;
	int x[8],x1[8],m[8][8];

	//Transformacija redova

	for(i = 0, k = 0; i < 8 ; i++, k +=8)
	{
		for(j = 0; j < 8; j++)
			x[j]=X[k+j];	//vrednosti jednog reda

		for(j = 0; j < 4; j++)  //prvi deo transformacije
		{
			j1= 7 - j;
            x1[j]=x[j] + x[j1];
            x1[j1]= x[j] - x[j1];
		}
		x[0] = x1[0] +x1[3]; //drugi deo transformacije
		x[1] = x1[1] +x1[2];
		x[2] = x1[1] -x1[2];
		x[3] = x1[0] -x1[3];
		x[4] = x1[4]; 
		//posle mnozenja dodaje se delta  radi zaokruzivanja
		x[5]= ((x1[6] - x1[5]) * c0 + delta ) >> shift;
		x[6]= ((x1[6] + x1[5]) * c0 + delta ) >> shift;
		x[7]= x1[7];

		m[i][0] = (x[0] +x[1])*c4; //gornja polovina treceg dela transformacije 
		m[i][4] = (x[0] -x[1])*c4;
		m[i][2] = x[2] * c6 + x[3] * c2;
		m[i][6] = x[3] * c6 - x[2] * c2;

		x1[4] = x[4] + x[5]; //donja polovina od treceg dela tranformacije
		x1[5] = x[4] - x[5];
		x1[6] = x[7] - x[6];
		x1[7] = x[7] + x[6];

		m[i][1] = x1[4] * c7 + x1[7] * c1; // donja polovin cetvrtog dela tranformacije
		m[i][7] = x1[7] * c7 - x1[4] * c1;
		m[i][5] = x1[5] * c3 + x1[6] * c5;
		m[i][3] = x1[6] * c3 - x1[5] * c5;
	}

	//transformacija kolona
	for (i = 0; i < 8; i++) //osam kolona
	{
		for (j = 0; j < 4; j++)//uzimamo jednu kolonu 
		{
			j1 = 7 - j;			//operacije u prvom delu trnasformacije
			x1[j] = m[j][i] + m[j1][i];
			x1[j1] = m[j][i] - m[j1][i];
		}

							//operacije u drugome delu tranformacije
		x[0] = x1[0] + x1[3];
		x[1] = x1[1] + x1[2];
		x[2] = x1[1] - x1[2];
		x[3] = x1[0] - x1[3];
		x[4] = x1[4];
							//siftujemo x[5] i x[6], da nemi doslo do prekoracenja
		x1[5] = (x1[5] + delta) >> shift;
		x1[6] = (x1[6] + delta) >> shift;
		x[5] = (x1[6] - x1[5]) * c0;
		x[6] = (x1[6] + x1[5]) * c0;
		x[7] = x1[7];

		m[0][i] = (x[0] + x[1]) * c4; // gornja polovina treceg konacnog dela tranformacije
		m[4][i] = (x[0] - x[1]) * c4;
		m[2][i] = ( x[2] * c6 + x[3] * c2);
		m[6][i] = ( x[3] * c6 - x[2] * c2);

		x1[4] = x[4] + x[5]; //donja poloina treceg dela
		x1[7] = x[7] + x[6];
		x1[5] = x[4] - x[5];
		x1[6] = x[7] - x[6];

		m[1][i] = x1[4] * c7 + x1[7] * c1; //donja polovina cetvrtog dela
		m[5][i] = x1[5] * c3 + x1[6] * c5;
		m[3][i] = x1[6] * c3 - x1[5] * c5;
		m[7][i] = x1[7] * c7 - x1[4] * c1;
	}//za i
	
	for( i = 0 ; i <  8; ++i)
	{
		for(j = 0; j < 8; ++j)
		{
			*Y++ = (m[i][j] + delta1) >> shift1; //zaokruzujemo dodavanjem delte
		}
	}

}

void idct(int *Y, int *X)
{
	int j1, i, j, k;
	int x[8], x1[8], m[8][8], y[8];
	k = i = 0;	
	
	//tranformacije kolona
	for ( i = 0; i < 8; ++i )
	{
		for (j = 0; j < 8; j++)
			y[j] = Y[i+8*j];

		x1[4] = y[1] * c7 - y[7] * c1; //donja polovina konacnog dela dct
		x1[7] = y[1] * c1 + y[7] * c7;
		x1[6] = y[3] * c3 - y[5] * c5;
		x1[5] = -y[3] * c5 + x1[4] * c3;

		x[4] = (x1[4] + x1[5]); //donja polovina treceg dela dct
		x[5] = (x1[4] - x1[5]);
		x[6] = (x1[1] - x1[6]);
		x[7] = (x1[7] + x1[6]);
		

		x1[0]= (y[0] + y[4]) * c4; //gornja polovina 3 dela koacnog dct
		x1[1]= (y[0] - y[4]) * c4;
		x1[2]= y[2] * c6 - y[6] * c2;
		x1[3]= y[2] * c2 + y[6] * c6;

		x[0] = x1[0] + x1[3]; //drugi deo dct
		x[1] = x1[1] + x1[2];
		x[2] = x1[1] - x1[2];
		x[3] = x1[0] - x1[3];

		x1[5] = (( x[6] - x[5]) * c0 + delta) >> shift;
		x1[6] = (( x[6] + x[5]) * c0 + delta) >> shift;
		x[5] = x1[5];
		x[6] = x1[6];

		for (j = 0; j < 4; j++) //prvi deo dct
		{
			j1 = 7 - j;
			m[j][i] = (x[j] + x[j1] );
			m[j1][i] = (x[j] - x[j1] );
		}
	}
	
	//transformacija reda
	for ( i = 0; i < 8; i++ ) 
	{
		for (j = 0; j < 8; j++)
			y[j] = m[i][j] ; //vrednosti za jedan red

		x1[4]= y[1] * c7 - y[7] * c1;
		x1[7]= y[1] * c1 + y[7] * c7;
		x1[6]= y[3] * c3 + y[5] * c5;
		x1[5]= -y[3] *c5 + y[5] * c3;

		
		x[4] = (x1[4] + x1[5]); //donja polovina treceg dela dct
		x[5] = (x1[4] - x1[5]);
		x[6] = (x1[1] - x1[6]);
		x[7] = (x1[7] + x1[6]);


		x1[0]= (y[0] + y[4]) * c4; //gornja polovina 3 dela koacnog dct
		x1[1]= (y[0] - y[4]) * c4;
		x1[2]= y[2] * c6 - y[6] * c2;
		x1[3]= y[2] * c2 + y[6] * c6;

		x1[5] = (x[5] + delta) >> shift;
		x1[6] = (x[6] + delta) >> shift;
		x[5] = (x1[6] - x1[5]) * c0;
		x[6] = (x1[6] + x1[5]) * c0;
		
		//x[4], x[7] bez promena
		x[0] = ( x1[0] + x1[3] );
		
		//drugi deo transormacije
		x[1] = ( x1[1] + x1[2] );
		x[2] = ( x1[1] - x1[2] );
		x[3] = ( x1[0] - x1[3] );

		for (j = 0; j < 4; j++) //prvi deo transformacije
		{
			j1 = 7 - j;
			m[i][j] = (x[j] + x[j1]);
			m[i][j1] = (x[j] - x[j1]);
		}
	}

	
	for ( i = 0; i < 8; ++i ) 
	{
		for ( j = 0; j < 8; ++j ) 
		{
			*X++ = (m[i][j] + delta1) >> shift1;//zaokruivanje dodavanje delte
		}
	}
}


