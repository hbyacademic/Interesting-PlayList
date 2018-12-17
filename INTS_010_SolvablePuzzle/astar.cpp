#include <iostream>
#include <fstream> 
#include <stdlib.h>
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

void setcolor(Image m,int x,int y,int bow)  // 0 -> white ; 1 -> black
{
	if(bow==0)
		m.term[y*m.rowsize+x]=m.term[y*m.rowsize+x+1]=m.term[y*m.rowsize+x+2]=BYTE(255);
	else
		m.term[y*m.rowsize+x]=m.term[y*m.rowsize+x+1]=m.term[y*m.rowsize+x+2]=BYTE(0);
}

int row[9]={0,1,2,0,1,2,0,1,2};
int col[9]={2,2,2,1,1,1,0,0,0};

vector<int> result;
int init[9]={1,4,2,7,5,3,8,9,6};
/*****
1 4 2
7 5 3
8 9 6
*****/

//count the # of inversions in the puzzle 
int inversion()
{
	int count=0,flag;
	for(int i=1;i<sizeof(init)/sizeof(init[0]);i++)
	{
		for(int j=i;j<sizeof(init)/sizeof(init[0]);j++)
		{
			flag=init[i];
			if(flag>init[j])	
				count++;
		}
	}
	return count;
}

//flow chart of A* algorithm
//rand -> find_white -> neighbor -> h -> comp -> swapNupdate -> game_over

Image rand(Image input)
{
	Image output(input.height,input.width);
	
	//set the top left corner to be the white subpuzzle
	for(int y=col[0]*input.height/3;y<(col[0]+1)*input.height/3;y++)
		for(int x=row[0]*input.rowsize/3;x<(row[0]+1)*input.rowsize/3;x+=3)
			setcolor(output,x,y,0);
	
	//generate the random number from 2~9
	//for(int i=1;i<10;i++) init.push_back(i);
	//random_shuffle(init.begin()+1,init.end());
	

	//shuffle eight subpuzzles for game start
	int seq=2;
	for(int j=1;j<sizeof(init)/sizeof(init[0]);j++)
	{
		for(int yy=col[init[j]-1]*input.height/3,y=col[seq-1]*input.height/3;yy<(col[init[j]-1]+1)*input.height/3 && y<(col[seq-1]+1)*input.height/3;yy++,y++)
			for(int xx=row[init[j]-1]*input.rowsize/3,x=row[seq-1]*input.rowsize/3;xx<(row[init[j]-1]+1)*input.rowsize/3 && x<(row[seq-1]+1)*input.rowsize/3;xx+=3,x+=3)
			{
				output.term[y*input.rowsize+x]=BYTE(input.term[yy*input.rowsize+xx]);
				output.term[y*input.rowsize+x+1]=BYTE(input.term[yy*input.rowsize+xx+1]);
				output.term[y*input.rowsize+x+2]=BYTE(input.term[yy*input.rowsize+xx+2]);
			}
		seq++;
	}
	//init.clear();
	return output;
}

//find the position of 1 (white subpuzzle)
int find_white()
{
	for(int i=0;i<sizeof(init)/sizeof(init[0]);i++)
		if(init[i]==1) 
			return (i+1);
}

/***********************************************************
1 2 3
4 5 6 -> neighbors of 1 is 2 and 4 as shown in neighbor[0]
7 8 9
***********************************************************/
int neighbor[10]={0,24,135,26,157,2468,359,48,759,68};

//calculate the h value of the certain step
int h(int neighbor)
{
	vector<int>tmp;
	tmp.clear();
	for(int i=0;i<sizeof(init)/sizeof(init[0]);i++)
		tmp.push_back(init[i]);
		
	int white,offset=0;
	//find the position of 1 (represents the white subpuzzle)
	for(int i=0;i<tmp.size();i++)
		if(tmp[i]==1) white=i;
	
	//white -> w ; neighbor -> n
	/****************************
		w 2 3     swap    n 2 3  
		n 5 6    ------>  w 5 6 
	    7 8 9             7 8 9
	****************************/
	swap(tmp[white],tmp[neighbor-1]);
	
	//calculate the h value of state after swapping
	/****************************
		w 2 3     goal    w 2 3  
		5 4 6    ------>  4 5 6 
	    7 8 9             7 8 9
	****************************/
	// h value 
	// = offset of 4 from current to goal state + offset of 5 from current to goal state
	// = 1+1 =2 
	for(int i=2;i<10;i++)
		for(int j=0;j<tmp.size();j++)
		if(tmp[j]==i)
		{
			offset+=abs(j/3-(i-1)/3);
			offset+=abs(j%3-(i-1)%3);
		}
	return offset;
}

//find the minimal h value of every valid neighbor
int comp(int all_neighbor)
{
	vector<int>w;
	vector<int>all_h_value;
	all_h_value.clear();
	int copy=all_neighbor,step;
	while(copy>0)
	{	//record the h value "h(copy%10)" and the chosen subpuzzle "copy%10"
		all_h_value.push_back(h(copy%10));
		all_h_value.push_back(copy%10);
		copy/=10;
	}
	
	int min=INT_MAX;
	//find the minimal value 
	for(int i=0;i<all_h_value.size();i+=2)
		if(all_h_value[i]<min)// && all_h_value[i+1]!=((w.size()<2)?1:w[w.size()-2])) bug here <-----
		{
			min=all_h_value[i];
			step=all_h_value[i+1];	
		}
	w.push_back(step);

	//return the chosen subpuzzle index	
	return step;
}

//swipes two subpuzzles and update the init array  
Image swapNupdate(int step,Image input)
{
	Image tmp(input.height,input.width);
	int white,seq=1;
	for(int i=0;i<sizeof(init)/sizeof(init[0]);i++)
		if(init[i]==1) white=i;
		
	swap(init[white],init[step-1]);
			
	//move the chosen one to location of white subpuzzle
	for(int yy=col[white]*input.height/3,y=col[step-1]*input.height/3;yy<(col[white]+1)*input.height/3 && y<(col[step-1]+1)*input.height/3;yy++,y++)
		for(int xx=row[white]*input.rowsize/3,x=row[step-1]*input.rowsize/3;xx<(row[white]+1)*input.rowsize/3 && x<(row[step-1]+1)*input.rowsize/3;xx+=3,x+=3)
		{
			input.term[yy*input.rowsize+xx]=BYTE(input.term[y*input.rowsize+x]);
			input.term[yy*input.rowsize+xx+1]=BYTE(input.term[y*input.rowsize+x+1]);
			input.term[yy*input.rowsize+xx+2]=BYTE(input.term[y*input.rowsize+x+2]);
		}
		
	//set the chosen region to white
	for(int y=col[step-1]*input.height/3;y<(col[step-1]+1)*input.height/3;y++)
		for(int x=row[step-1]*input.rowsize/3;x<(row[step-1]+1)*input.rowsize/3;x+=3)
			setcolor(input,x,y,0);

	//record the step 
	result.push_back(step);
	tmp=input;
	return tmp;
}

//game over
int game_over()	 
{
	int i;
	//if the init sequence matches "123456789"
	/*****************************************
	 			  	 represents   1 2 3
	 	"123456789" ---------->   4 5 6 
	 		 				      7 8 9
	*****************************************/  
	for(i=0;i<sizeof(init)/sizeof(init[0]);i++)
		if(init[i]!=(i+1))
			break;
	
	return (i<sizeof(init)/sizeof(init[0]))? 0 : 1;
}

int main()
{ 
	system("mode con: lines=80 cols=55");
	Image input,output,tmp;
	char filename[80];
	
	//select an image
	cout<<"Input an image: ";
	cin>>filename;
	input.load(filename);
	
	cout<<"# of inversions in the puzzle: "<<inversion()<<endl;
	if(inversion()%2==0)
	{
		//remove the left top corner & randomly set the locations of eight subpuzzles
		output=rand(input);
		input=output;
		input.save("statement.bmp");
	
		cout<<"Let's 'take a look for at the statement..."<<endl;
		system("PAUSE");
		
		cout<<"Moving Puzzle solved by A* search algorithm"<<endl;
		
		while(1)
		{	
			static int cnt=49;
			char fname[]="stepx.bmp";
			fname[4]=cnt;
			//record each step  
			tmp=swapNupdate(comp(neighbor[find_white()]),input);
			tmp.save(fname);
			if(game_over())
			{
				cout<<"Step: ";
				for(int i=0;i<result.size();i++)
					cout<<result[i]<<" ";
				cout<<endl;
				break;
			}
			cnt++;	
		}
	}
	
	else 
		cout<<"This puzzle is unsolvable !!!"<<endl;
	return 0;
}

