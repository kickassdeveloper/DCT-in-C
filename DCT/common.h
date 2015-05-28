//common.h
//definicija RGB pixela 

class RGB{
	public:
		unsigned char R;  //0-255
		unsigned char G;  //0-255
		unsigned char B;  //0-255
};

class YCbCr{
	public:
		unsigned char Y;  //0-255
		unsigned char Cb; //0-255
		unsigned char Cr; //0-255
};

//4:2:0 YCbCr Markroblok
class YCbCr_MACRO{
	public:
		unsigned char Y[256]; //16x16 ( cetri 8x8 matrice)
		unsigned char Cb[64]; //8x8 
		unsigned char Cr[64]; //8x8
};

class RGBImage{
	public:
		short width; // sirina slike
		short height;//duzina slike
		unsigned char *ibuf; //referenca prema sadrzaju slike
};
