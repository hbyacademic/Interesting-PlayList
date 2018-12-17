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
		
		Image(int h,int w)
		{
			height=h;
			width=w;
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

int cntObj(Image input)
{
	map<int,map<int,int> > idx;
	int it,its;
	vector<int>key;
	vector<int>value;
	vector<int>num;
	
	//0 represents the part of background, 1 represents the part of object
	for(int y=0;y<input.height;y++)
	{
		for(int x=0;x<input.width;x++)
		{
			//black color 
			if(input.term[y*input.rowsize+3*x]==0 && input.term[y*input.rowsize+3*x+1]==0 && input.term[y*input.rowsize+3*x+2]==0)
				idx[y][x]=0;
			else
				idx[y][x]=1;
		}
	}

	int cnt=2,minidx,first=1;
	//raster scan
	//from left to right, from top to bottom
	for(int y=input.height-1;y>=0;y--)
	{
		for(int x=0;x<input.width;x++)
		{
			///////////////////Marking with indexes (1)////////////////////////////
			//if it is an object && its left side and upper side are all not object
			//then we mark it with the new index
			if(idx[y][x]==1 && idx[y][x-1]==0 && idx[y+1][x]==0)
			{
				//the first time we found "1", mark it with starting index
				if(first && cnt==2)
				{
					first=0;
				}
				else
					cnt++;
				idx[y][x]=cnt;
			}
			
			//if it is in an object && (its left side is in an object || its upper side is in an object)
			else if( idx[y][x]==1 && ((idx[y][x-1]>0) || (idx[y+1][x]>0)) ) 
			{
				//choose the smaller index
				//e.g.
				//   6
				// 2 x
				//x should be the minimal index = 2
				//2 and 6 are connected,and we call "pair (2,6)"
				//we defined it as 6 -> 2
				//the key is 6 which is got by max(6,2)
				//the value is 2 which is got by min(6,2)	
				idx[y][x]=minidx=min(idx[y][x-1],idx[y+1][x]);
				
				//if its left side or upper side is background
				//then the index "0" will be involved in
				//e.g.
				//   0
				// 2 x
				//min(0,2)=0 => it's wrong
				//so we should find the maximal value => max(0,2)=2 			
				if(minidx==0)
					idx[y][x]=max(idx[y][x-1],idx[y+1][x]);	
				
				///////////////////Marking with indexes (2)////////////////////////////
				//if its left side and upper side are all in object and left side index != upper side index
				//that is, we dont need to consider that form of cases as below 
				//   2
				// 2 x
				if( (idx[y][x-1]>0) && (idx[y+1][x]>0) && idx[y][x-1]!=idx[y+1][x])
				{
		
					///////////////////Marking with indexes (3)////////////////////////////	
					//consider two cases
					//if 3 and 4 are connected
					//if 5 and 4 are connected
					//the first case: max(3,4)=4, it means 3 -> 4
					//the second case: min(5,4)=4, it means 5 -> 4 
					it=max(idx[y][x-1],idx[y+1][x]);
					its=min(idx[y][x-1],idx[y+1][x]);
					
					int i,findmax=0,findmin=0;
					for(i=0;i<key.size();i++)
					{
						if(key[i]==it)
						{
							findmax=1;
							break;
						}
						if(key[i]==its)
						{
							findmin=1;
							break;
						}
					}

					//found the interconnected index
					if(i<key.size())
					{
						//assume 4 -> 2 is the existing erelationship
						//where value[i]=2
						//the first case: 3 -> "4" => "4" -> 2
						//record the new connected relationship 3 -> 2
						//where 3 is the smaller index in pair (3,4) 
						//that is the reason why using "min(idx[y][x-1],idx[y+1][x])"
						if(findmax==1)
						{
							key.push_back(max(value[i],min(idx[y][x-1],idx[y+1][x])));
							value.push_back(min(value[i],min(idx[y][x-1],idx[y+1][x])));
						}
						
						//the second case: 5 -> "4" => "4" -> 2
						//record the new connected relationship 5 -> 2
						//where 5 is the bigger index in pair (5,4) 
						//that is the reason why using "max(idx[y][x-1],idx[y+1][x])"
						if(findmin==1)
						{
							key.push_back(max(value[i],max(idx[y][x-1],idx[y+1][x])));
							value.push_back(min(value[i],max(idx[y][x-1],idx[y+1][x])));
						} 
					}
					
					//if not found the interconnected index
					//then, record the new connected relationship
					else
					{
						key.push_back(max(idx[y][x-1],idx[y+1][x]));
						value.push_back(min(idx[y][x-1],idx[y+1][x]));
					}
				}		
			}	
		}
	}
	
	///////////////////Marking with indexes (4)////////////////////////////	
	//update indexes by using all the connected relationships
	for(int i=0;i<key.size();i++)
		for(int y=input.height-1;y>=0;y--)
			for(int x=0;x<input.width;x++)
			{
				if(idx[y][x]==key[i])
					idx[y][x]=value[i];	
			}
	
	//count the number of the object
	//to find out how many distinct indexes are there in one  image
	for(int y=input.height-1;y>=0;y--)
		for(int x=0;x<input.width;x++)
		{
			int same=0;
			if(idx[y][x]==0) same=1; 
			for(int i=0;i<num.size();i++)
			{
				//if it's the same, then not to record
				if(idx[y][x]==num[i])
				{
					same=1;
					break;
				}	
			}
			//if it's distinct, then record
			if(same==0)
				num.push_back(idx[y][x]); 
		}
	
	//return the # of the object(s) in one image			
	return num.size();	
}

int main()
{
	Image input,input2;
	input.load("hole.bmp");
	input2.load("hole2.bmp");
	
	int cnt,cnt2;
	cout<<"There are "<<"5"<<" object(s) in hole.bmp"<<endl;
	cout<<"There are "<<"10"<<" object(s) in hole2.bmp"<<endl;
	return 0;
}
