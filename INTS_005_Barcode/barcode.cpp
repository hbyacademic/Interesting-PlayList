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

void setcolor(Image m,int x,int y,int bow)  // 0 -> white ; 1 -> black
{
	if(bow==0)
		m.term[y*m.rowsize+x]=m.term[y*m.rowsize+x+1]=m.term[y*m.rowsize+x+2]=BYTE(255);
	else
		m.term[y*m.rowsize+x]=m.term[y*m.rowsize+x+1]=m.term[y*m.rowsize+x+2]=BYTE(0);
}

Image barcode(Image input,vector<int>str) 
{
	Image output=Image(input.height,input.width);
	for(int y=0;y<input.height;y++)
		for(int x=0;x<input.rowsize;x+=3)
		{
			//leaving white 
			if(y>34 || y<5)	
				setcolor(output,x,y,0);
			
			else if(y==5 || y==6)
			{
				//starting, ending, middle symbol should be longer
				if(x/3==2 || x/3==3 || x/3==4 || x/3==47 || x/3==48 || x/3==49 || x/3==50 || x/3==51 || x/3==94 || x/3==95 || x/3==96) 
					setcolor(output,x,y,str[x/3]);	
				else	
					setcolor(output,x,y,0);
			}
			//follow the binary sequence
			else
				setcolor(output,x,y,str[x/3]);
		}

	return output;
}

int main()
{ 
	//setting the height & width of the barcode 
	Image inputt=Image(40,99);
	Image output;
	vector<int>str;
	
	//rule for the first six digits 
	//0 -> type A ; 1 -> type B
	int rule[10][6]={
		0,0,0,0,0,0,
		0,0,1,0,1,1,
		0,0,1,1,0,1,
		0,0,1,1,1,0,	
		0,1,0,0,1,1,
		0,1,1,0,0,1,
		0,1,1,1,0,0,
		0,1,0,1,0,1,
		0,1,0,1,1,0,
		0,1,1,0,1,0
	};
	
	//[3] -> type A B C 
	//[10] -> 0~9
	//[7] -> 7 bits
	int abc[3][10][7]={	
		0,0,0,1,1,0,1, 0,0,1,1,0,0,1, 0,0,1,0,0,1,1, 0,1,1,1,1,0,1, 0,1,0,0,0,1,1, 0,1,1,0,0,0,1, 0,1,0,1,1,1,1, 0,1,1,1,0,1,1, 0,1,1,0,1,1,1, 0,0,0,1,0,1,1,
		0,1,0,0,1,1,1, 0,1,1,0,0,1,1, 0,0,1,1,0,1,1, 0,1,0,0,0,0,1, 0,0,1,1,1,0,1, 0,1,1,1,0,0,1, 0,0,0,0,1,0,1, 0,0,1,0,0,0,1, 0,0,0,1,0,0,1, 0,0,1,0,1,1,1,
		1,1,1,0,0,1,0, 1,1,0,0,1,1,0, 1,1,0,1,1,0,0, 1,0,0,0,0,1,0, 1,0,1,1,1,0,0, 1,0,0,1,1,1,0, 1,0,1,0,0,0,0, 1,0,0,0,1,0,0, 1,0,0,1,0,0,0, 1,1,1,0,1,0,0
	};
	
	//initialize the input barcode  
	char input[13]=""; 	
	
	//starting symbol (first two bits for leaving two spaces)	
	int start[5]={0,0,1,0,1};
	//middle symbol	
	int mid[5]={0,1,0,1,0};
	//ending symbol (last two bits for leaving two spaces)		
	int end[5]={1,0,1,0,0};	
	
	//input the number sequence for generating the barcode
	for(int i=0;i<13;i++)
		cin>>input[i];
	
	//put the starting bits into vector
	for(int i=0;i<5;i++)
		str.push_back(start[i]);
		
	for(int i=1;i<13;i++)
	{
		//import digit (input[0]) determines the encoding rules of the next six digits
		if(i<7)
		{ 
			for(int j=0;j<7;j++)	
				str.push_back(abc[rule[input[0]-'0'][i-1]][input[i]-'0'][j]);
			
			//put the middle symbol bits into vector
			if(i==6)	
				for(int i=0;i<5;i++)
					str.push_back(mid[i]);
		} 
		//other digits follow the type C
		else
			for(int j=0;j<7;j++)
				str.push_back(abc[2][input[i]-'0'][j]);	
	}
	//put the ending symbol bits into vector
	for(int i=0;i<5;i++)
		str.push_back(end[i]);

	//use the binary sequence stored in vector to generate the barcode
	output=barcode(inputt,str);
	output.save("barcode.bmp");
	return 0;
}

