#include <iostream>                             
#include <fstream>  // file processing  		
#include <time.h>   //srand using 				
#include<stdio.h>
#include<stdlib.h>  //random using   
#include<cmath>		//abs using
#include<vector>
using namespace std;

typedef unsigned char   BYTE;   //  1 byte (0~255) 
typedef unsigned short  WORD;   //  2 bytes (0~65536) 
typedef unsigned long   DWORD;  //  4 bytes (0~2^32 -1) 
								// int -> 4 bytes
																
#pragma pack(push)  //store the original setting for recovering using 
#pragma pack(2)    //aligned to 2 bytes  
struct INFO
{
	// BITMAPFILEHEADER (14 bytes) from 16 reducing to 14
	WORD bfType;          //BM -> 0x4d42 (19778) => because of xxx.bmp     
	DWORD BfSize;         //total size of image        
	WORD bfReserved1;     //bfReserved1 must set to be 0
    WORD bfReserved2;     //bfReserved2 must set to be 0        
    DWORD bfOffBits;      //offset 
	// BITMAPINFOHEADER(40 bytes)    
    DWORD biSize;         //size of info header    
    int biWidth;		  //width of the image
    int biHeight;		  //height of the image
    WORD biPlanes;        //位元圖層數=1 
    WORD biBitCount;      //bit per pixel (bpp) 
    DWORD biCompression;  //0 represents "NO" compression
    DWORD biSizeImage;    //bitmap data size  
    int biXPelsPerMeter;  //horizontal resolution 
    int biYPelsPerMeter;  //vertical resolution
    DWORD biClrUsed;      //0 for all colors in palette used
    DWORD biClrImportant; //0 for all colors are equally important 
};
#pragma pack(pop)    //recover the original setting 

void print(INFO h)	//informations of the image 
{         
	cout<<"bfType: "<<h.bfType<<endl;
	cout<<"bfSize: "<<h.BfSize<<endl;
	cout<<"bfReserved1: "<<h.bfReserved1<<endl;
	cout<<"bfReserved2: "<<h.bfReserved2<<endl;
	cout<<"bfOffBits: "<<h.bfOffBits<<endl;
	cout<<"biSize: "<<h.biSize<<endl;
	cout<<"biWidth: "<<h.biWidth<<endl;
	cout<<"biHeight: "<<h.biHeight<<endl;
	cout<<"biPlanes: "<<h.biPlanes<<endl;
	cout<<"biBitCount: "<<h.biBitCount<<endl;
	cout<<"biCompression: "<<h.biCompression<<endl;
	cout<<"biSizeImage: "<<h.biSizeImage<<endl;
	cout<<"biXPelsPerMater: "<<h.biXPelsPerMeter<<endl;
	cout<<"biYPelsPerMeter: "<<h.biYPelsPerMeter<<endl;
	cout<<"biClrUsed: "<<h.biClrUsed<<endl;
	cout<<"biClrImportant: "<<h.biClrImportant<<endl;
}

class Image
{
	
	public:
		
		int height;
		int width;
		int rowsize;    // bgr -> 3 bytes(24 bits) 
		BYTE* term;
		
		Image()   //storage is bottom-up,and from left to right 
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
			for(int y=0;y<height;y++)
				for(int x=0;x<rowsize;x++)
					term[y*rowsize+x]=BYTE(255);
		}
		
		void load(const char *filename)	//char* filename => const char* filename 
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
		
		void save(const char* filename)	//char* filename => const char* filename 
		{
			INFO h=
			{		
				19778,   //0x4d42
				DWORD(54+rowsize*height), 	//convert int to DWORD  
				0,
				0,
				54,
				40,
				width,
				height,
				1,
				24,	   
				0,
				DWORD(rowsize*height),		//convert int to DWORD 
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
 
Image rotate(Image input,double degree)
{
	//degree => radian
	double theta=degree*M_PI/180.0;
	
	//four corner of the image
	int x1,y1,x2,y2,x3,y3,x4,y4;
	//(x3,y3) => upper left; (x4,y4) => upper right;	 
	//(x1,y1) => lower left; (x2,y2) => lower right; 
	x1=0; y1=0;	
	x2=input.width-1; y2=0;	
	x3=0; y3=input.height-1;	
	x4=input.width-1; y4=input.height-1;
	
	//(x0,y0) => middle point of the image
	int x0=(input.width-1)/2,y0=(input.height-1)/2;
	//rotating center from (0,0) to (x0,y0)
	x1-=x0; y1-=y0;
	x2-=x0; y2-=y0;
	x3-=x0; y3-=y0;
	x4-=x0; y4-=y0;
	
	//four corner after rotating ,in which rotate center (x0,y0)
	int nx1,ny1,nx2,ny2,nx3,ny3,nx4,ny4;
	nx1=x1*cos(theta)-y1*sin(theta); ny1=x1*sin(theta)+y1*cos(theta);
	nx2=x2*cos(theta)-y2*sin(theta); ny2=x2*sin(theta)+y2*cos(theta);
	nx3=x3*cos(theta)-y3*sin(theta); ny3=x3*sin(theta)+y3*cos(theta);
	nx4=x4*cos(theta)-y4*sin(theta); ny4=x4*sin(theta)+y4*cos(theta);
	
	//find the new size of the image after rotating
	int maxx,maxy,minx,miny;
	minx=min(nx1,min(nx2,min(nx3,nx4)));
	miny=min(ny1,min(ny2,min(ny3,ny4)));
	maxx=max(nx1,max(nx2,max(nx3,nx4)));
	maxy=max(ny1,max(ny2,max(ny3,ny4)));
	
	//define the new size of the image
	Image output(maxy-miny,maxx-minx);

	//backtrace (reversely rotate then reversely offset) 
	int orgx,orgy;
	for(int y=miny;y<maxy;y++)
		for(int x=minx;x<maxx;x++)
		{
			//reversely rotate
			orgx=x*cos(theta*-1.0)-y*sin(theta*-1.0);
			orgy=x*sin(theta*-1.0)+y*cos(theta*-1.0);
			
			//reversely offset
			orgx+=x0;
			orgy+=y0;
			
			//if the pixel is NOT in the region of original image, then set to be white color
			if(orgx<0 || orgx>input.width || orgy<0 || orgy>input.height)
				output.term[(y-miny)*output.rowsize+3*(x-minx)]
				=output.term[(y-miny)*output.rowsize+3*(x-minx)+1]
				=output.term[(y-miny)*output.rowsize+3*(x-minx)+2]
				=255;
				
			//otherwise, copy the color to corresponding pixel
			else
			{
				output.term[(y-miny)*output.rowsize+3*(x-minx)]=input.term[orgy*input.rowsize+3*orgx];	
				output.term[(y-miny)*output.rowsize+3*(x-minx)+1]=input.term[orgy*input.rowsize+3*orgx+1];
				output.term[(y-miny)*output.rowsize+3*(x-minx)+2]=input.term[orgy*input.rowsize+3*orgx+2];
			}	
		}
	return output;
} 
 
int main()
{
	Image input,output;
	input.load("face.bmp");
	int degree;
	cout<<"Input an degree for rotating: ";
	cin>>degree; 
	output=rotate(input,degree);
	output.save("output.bmp");
	
	return 0;
}
