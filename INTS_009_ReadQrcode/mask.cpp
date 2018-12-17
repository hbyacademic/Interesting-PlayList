#include <iostream>
#include <fstream> 
#include<vector>
using namespace std;

typedef unsigned char   BYTE;   //  1 byte (0~255) 
typedef unsigned short  WORD;   //  2 bytes (0~65536) 
typedef unsigned long   DWORD;  //  4 bytes (0~2^32 -1) 

#pragma pack(push) 
#pragma pack(2)    
struct INFO
{
	// BITMAPFILEHEADER (14 bytes) from 16 reducing to 14
	WORD bfType;          //BM -> 0x4d42 (19778)     
	DWORD BfSize;         //�`���ɤj�p        
	WORD bfReserved1;     //bfReserved1 ����0  
    WORD bfReserved2;     //bfReserved2 ����0        
    DWORD bfOffBits;      //�����q 
	// BITMAPINFOHEADER(40 bytes)    
    DWORD biSize;         //info header�j�p     
    int biWidth;
    int biHeight;
    WORD biPlanes;        //�줸�ϼh��=1 
    WORD biBitCount;      //�C��pixel�ݭn�h��bits 
    DWORD biCompression;  //0�������Y 
    DWORD biSizeImage;    //�I�}�ϸ�Ƥj�p  
    int biXPelsPerMeter;  //�����ѪR�� 
    int biYPelsPerMeter;  //�����ѪR�� 
    DWORD biClrUsed;      //0���ϥΩҦ��զ�L�C�� 
    DWORD biClrImportant; //���n���C���(0���Ҧ��C��Ҥ@�˭��n) 
};
#pragma pack(pop) 

class Image
{	
	public:
		
		int height;
		int width;
		int rowsize;    // bgr -> 3 bytes(24 bits) 
		BYTE* term;
		
		Image()   //storage is bottom-up,from left to right 
		{
			height=0;
			width=0;
		}
		
		Image(int height,int width)
		{
			this->height=height;
			this->width=width;
			rowsize=(3*width+3)/4*4;   //set to be a multiple of "4" 
			term=new BYTE[height*rowsize];  
		}
		
		void load(const char *filename)
		{
			INFO h;  
			ifstream f;
			f.open(filename,ios::in|ios::binary);
			f.seekg(0,f.end);
			//cout<<"���ɤj�p�G "<<f.tellg()<<"bytes"<<endl;
			f.seekg(0,f.beg);
			f.read((char*)&h,sizeof(h));
						
			width=h.biWidth;
			height=h.biHeight;
			//cout<<"reading from "<<filename<<"..."<<endl;
			//print(h);
			*this=Image(height,width);
			f.read((char*)term,height*rowsize);
			f.close();
		}
		
		void save(const char* filename)
		{
			INFO h=
			{		
				19778,   //0x4d42
				DWORD(54+rowsize*height),   
				0,
				0,
				54,
				40,
				width,
				height,
				1,
				24,   
				0,
				DWORD(rowsize*height),
				3780,   //3780
				3780,   //3780
				0,
				0				
			};
			//cout<<"writing into "<<filename<<"..."<<endl;
			ofstream f;
			f.open(filename,ios::out|ios::binary);
			f.write((char*)&h,sizeof(h));
			f.write((char*)term,rowsize*height);
			f.close();	
		}	
};

void bw(Image input)
{
	for(int y=0;y<input.height;y++)
		for(int x=0;x<input.rowsize;x+=3)
		{
			if(	BYTE(input.term[y*input.rowsize+x])==255 && BYTE(input.term[y*input.rowsize+x+1])==255 && BYTE(input.term[y*input.rowsize+x+2])==0)
			{
				input.term[y*input.rowsize+x]=0;
				input.term[y*input.rowsize+x+1]=0;
				input.term[y*input.rowsize+x+2]=0;
			}
		} 
}

void mask(Image input)
{
	for(int y=0;y<input.height;y++)
		for(int x=0;x<input.rowsize;x+=3)
		{
			//Not cosidering the region of finder pattern,error correction info/level, leaving white 
			if(	((y<8) && (x/3<9))  ||
				((y==8 || y==9 || y==10 || y==11 || y==12) && x/3==6) ||
				((y>11) && (x/3<9)) ||
				((y==14) && (x/3==8 || x/3==9 || x/3==10 || x/3==11 || x/3==12)) ||
				((y>11) && (x/3>12))
			)	continue;
			
			//mask pattern (i+j)%3==0
			else if(((input.height-y-1)+x/3)%3==0)
			{
				//XOR operation 
				//black+black=white
				if(input.term[y*input.rowsize+x]==0 && input.term[y*input.rowsize+x+1]==0 && input.term[y*input.rowsize+x+2]==0)	
				{
					input.term[y*input.rowsize+x]=255;
					input.term[y*input.rowsize+x+1]=255;
					input.term[y*input.rowsize+x+2]=255;
				}
				//black+white=black
				else
				{
					input.term[y*input.rowsize+x]=0;
					input.term[y*input.rowsize+x+1]=0;
					input.term[y*input.rowsize+x+2]=0;
				}
			}	
		}	
} 

int main()
{
	Image input;
	input.load("qrcode.bmp");
	mask(input);
	bw(input);
	input.save("deocded.bmp");
	return 0;
}
