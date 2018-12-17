#include <iostream>
#include <fstream>  // file processing  
#include <stdlib.h>
#include <map>
#include <vector>
#include <algorithm>
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
	DWORD BfSize;         //總圖檔大小        
	WORD bfReserved1;     //bfReserved1 須為0  
    WORD bfReserved2;     //bfReserved2 須為0        
    DWORD bfOffBits;      //偏移量 
	// BITMAPINFOHEADER(40 bytes)    
    DWORD biSize;         //info header大小     
    int biWidth;
    int biHeight;
    WORD biPlanes;        //位元圖層數=1 
    WORD biBitCount;      //每個pixel需要多少bits 
    DWORD biCompression;  //0為不壓縮 
    DWORD biSizeImage;    //點陣圖資料大小  
    int biXPelsPerMeter;  //水平解析度 
    int biYPelsPerMeter;  //垂直解析度 
    DWORD biClrUsed;      //0為使用所有調色盤顏色 
    DWORD biClrImportant; //重要的顏色數(0為所有顏色皆一樣重要) 
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
			//cout<<"圖檔大小： "<<f.tellg()<<"bytes"<<endl;
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

Image pixelization(Image input,int n)
{
	//initial output image = input image
	Image output(input.height,input.width);
	for(int y=0;y<output.height;y++)
		for(int x=0;x<output.rowsize;x+=3)
		{
			output.term[y*output.rowsize+x]=input.term[y*input.rowsize+x];
			output.term[y*output.rowsize+x+1]=input.term[y*input.rowsize+x+1];
			output.term[y*output.rowsize+x+2]=input.term[y*input.rowsize+x+2];
		}
		
	int midx,midy;
	/******************************************************************
		0 1 2 3 4
		1
		2   x      =>  x is the central pixel 
		3		   =>  position of x = (n/2, n/2) = (5/2, 5/2) = (2,2)
		4
	*******************************************************************/	
	for(int y=0;y<input.height/n;y++)
		for(int x=0;x<input.rowsize/n;x+=3)
			for(int dy=-n/2;dy<=n/2;dy++)
				for(int dx=-n/2*3;dx<=n/2*3;dx+=3)
				{
					//every central pixel (midx,midy)
					midx=3*n/2+x*n;
					midy=  n/2+y*n;	
					if(midx+dx<0 || midx+dx>input.rowsize || midy+dy<0 || midy+dy>input.height) continue;
					//replace the neighboring pixels with its central pixels
					output.term[(midy+dy)*output.rowsize+(midx+dx)]=input.term[midy*input.rowsize+midx];	
					output.term[(midy+dy)*output.rowsize+(midx+dx)+1]=input.term[midy*input.rowsize+midx+1];
					output.term[(midy+dy)*output.rowsize+(midx+dx)+2]=input.term[midy*input.rowsize+midx+2];
				}
	return output;
}

int main()
{ 
	system("mode con: lines=20 cols=60");
	Image input,output;
	input.load("face.bmp");
		
	output=pixelization(input,11);
	output.save("MSK_11.bmp");
	
	output=pixelization(input,9);
	output.save("MSK_9.bmp");
	
	output=pixelization(input,7);
	output.save("MSK_7.bmp");
	
	output=pixelization(input,5);
	output.save("MSK_5.bmp");
	
	output=pixelization(input,3);
	output.save("MSK_3.bmp");
	
	return 0;
}

