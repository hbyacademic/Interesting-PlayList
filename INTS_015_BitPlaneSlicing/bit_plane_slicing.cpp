#include <iostream>
#include <fstream>  // file processing  
#include <stdlib.h>
#include <map>
#include <vector>
#include <algorithm>
#include <cmath>
using namespace std;

typedef unsigned char   BYTE;   //  1 byte (0~255) 
typedef unsigned short  WORD;   //  2 bytes (0~65536) 
typedef unsigned long   DWORD;  //  4 bytes (0~2^32 -1) 

// int -> 4 bytes
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

Image Bplane(Image input,int level)
{
	Image output(input.height,input.width);
	
	//copy input to ouptut
	for(int y=0;y<input.height;y++)
		for(int x=0;x<input.rowsize;x+=3)
			output.term[y*output.rowsize+x]=output.term[y*output.rowsize+x+1]=output.term[y*output.rowsize+x+2]=input.term[y*input.rowsize+x];
	
	//bit plane slicing 
	//to find the n-th bit is 0 or 1, where 0 represents black (set the value to 0), and 1 represents white (set the value to 255)
	int threshold=(1<<(level-1));
	for(int y=0;y<input.height;y++)
		for(int x=0;x<input.rowsize;x+=3)
			output.term[y*output.rowsize+x]=output.term[y*output.rowsize+x+1]=output.term[y*output.rowsize+x+2]=((output.term[y*output.rowsize+x]/threshold)%2)?255:0;
	
	return output;
}

Image stack(Image input[8],vector<int>tab)
{
	Image output(input[0].height,input[0].width);

	for(int y=0;y<input[0].height;y++)
		for(int x=0;x<input[0].rowsize;x+=3)	
		{
			int stackvalue=0;
			for(int i=0;i<tab.size();i++)
			{
				int idx=tab[i]-1;
				stackvalue+=((input[idx].term[y*input[0].rowsize+x]/255)*(1<<(idx)));
			}
			output.term[y*output.rowsize+x]=output.term[y*output.rowsize+x+1]=output.term[y*output.rowsize+x+2]=stackvalue;	
		}
	return output;	
}

int main()
{ 
	Image input,output,in1,in2,in3,in4,in5,in6,in7,in8;
	input.load("face.bmp");
	char fname[6]="1.bmp";
	
	//output 8 bit planes
	for(int i=1;i<=8;i++)
	{
		output=Bplane(input,i);
		fname[0]=(i+48);
		output.save(fname);
	}
	
	Image pack[8]={in1,in2,in3,in4,in5,in6,in7,in8};
	for(int i=1;i<=8;i++)
	{
		fname[0]=(i+48);
		pack[i-1].load(fname);
	}
	
	int optionlevel;
	vector<int>tab;
	cout<<"select the level(s) you want: ";
	int num;
	char ch;
	while(scanf("%d%c",&num,&ch)==2)
	{
		if(ch=='\n') break;
		tab.push_back(num);
	} 
	output=stack(pack,tab);
	output.save("stack.bmp");
	return 0;
}

