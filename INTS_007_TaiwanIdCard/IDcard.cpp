#include <iostream>
#include <fstream> 
#include <cmath>
#include <vector>
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

Image NNI(Image input,int w,int h)	//nearest neighbor interpolation
{
	Image output(h,w);
	int nx,ny;
	for(int y=0;y<output.height;y++)
		for(int x=0;x<output.rowsize;x+=3)	
		{
			nx=(int((int(x*input.width*1.0/output.width)+2)/3*3));
			ny=int(y*input.height*1.0/output.height);
			output.term[y*output.rowsize+x]=input.term[ny*input.rowsize+nx];
			output.term[y*output.rowsize+x+1]=input.term[ny*input.rowsize+nx+1];
			output.term[y*output.rowsize+x+2]=input.term[ny*input.rowsize+nx+2];
		}
	return output;
}

void porfile(Image input,Image picture,int w,int h)		//load an image to ID card
{
	int face_width=85, face_height=105;	//size of the porfile picture on ID card	
	for(int y=0,y1=h;y<picture.height,y1<h+face_height;y++,y1++)
		for(int x=0,x1=w*3;x<picture.rowsize,x1<(w+face_width)*3;x+=3,x1+=3)
		{
			input.term[y1*input.rowsize+x1]=picture.term[y*picture.rowsize+x];
			input.term[y1*input.rowsize+x1+1]=picture.term[y*picture.rowsize+x+1];
			input.term[y1*input.rowsize+x1+2]=picture.term[y*picture.rowsize+x+2];
		}
}

void ID(Image input,Image *IDnum,int w,int h)	//load the ID number to ID card
{
	int num_width=8, num_height=15;	//size of the digit on ID card
	int IDlength=10;	//length of the ID number
	Image chosen;
	for(int i=0;i<IDlength;i++)
		for(int y=0,y2=h;y<IDnum[0].height,y2<h+num_height;y++,y2++)
			for(int x=0,x2=(w+num_width*i)*3;x<IDnum[0].rowsize,x2<(w+num_width*(i+1))*3;x+=3,x2+=3)
			{
				chosen=IDnum[i];
				input.term[y2*input.rowsize+x2]=chosen.term[y*chosen.rowsize+x];
				input.term[y2*input.rowsize+x2+1]=chosen.term[y*chosen.rowsize+x+1];
				input.term[y2*input.rowsize+x2+2]=chosen.term[y*chosen.rowsize+x+2];		
			}
}

void gender_deter(Image input,Image gender, int w,int h)	//load the gender to ID card
{
	int g_width=8, g_height=12;	//size of the gneder template image on ID card
	for(int y=0,y2=h;y<gender.height,y2<h+g_height;y++,y2++)
		for(int x=0,x2=w*3;x<gender.rowsize,x2<(w+g_width)*3;x+=3,x2+=3)
		{
			input.term[y2*input.rowsize+x2]=gender.term[y*gender.rowsize+x];
			input.term[y2*input.rowsize+x2+1]=gender.term[y*gender.rowsize+x+1];
			input.term[y2*input.rowsize+x2+2]=gender.term[y*gender.rowsize+x+2];		
		}
}

void date(Image input,Image *datenum,int w,int w2,int w3,int h)	//load the date of birth to ID card
{
	Image chosen;
	int posx[6]={w,w+8,w2,w2+8,w3,w3+8};
	for(int i=0;i<sizeof(posx)/sizeof(posx[0]);i++)
		for(int y=0,y2=h;y<datenum[0].height,y2<h+15;y++,y2++)
			for(int x=0,x2=posx[i]*3;x<datenum[0].rowsize,x2<posx[i]*3+8*3;x+=3,x2+=3)
			{
				chosen=datenum[i];
				input.term[y2*input.rowsize+x2]=chosen.term[y*chosen.rowsize+x];
				input.term[y2*input.rowsize+x2+1]=chosen.term[y*chosen.rowsize+x+1];
				input.term[y2*input.rowsize+x2+2]=chosen.term[y*chosen.rowsize+x+2];
			}
}

bool check_id(char *str)	//check the id
{
	//rule for numbering the English alphabet
	int table[26]={10,11,12,13,14,15,16,17,34,18,19,20,21,22,35,23,24,25,26,27,28,29,32,30,31,33};	
	if((str[1]=='1' || str[1]=='2') && (1*(table[str[0]-'A']/10)+9*(table[str[0]-'A']%10)+8*(str[1]-'0')+7*(str[2]-'0')+6*(str[3]-'0')+5*(str[4]-'0')+4*(str[5]-'0')+3*(str[6]-'0')+2*(str[7]-'0')+1*(str[8]-'0')+1*(str[9]-'0'))%10==0)
		return true;
	return false;
}

int main()
{ 
	system("mode con: lines=20 cols=60");
	Image templete,profile,porfile_o,gender,alpha,n1,n2,n3,n4,n5,n6,n7,n8,n9,d1,d2,d3,d4,d5,d6;
	int year,month,day;
	char str[10],fname[80],afname[6]="1.bmp",usrname[80],gname[7]="s0.bmp"; 
	Image IDnum[10]={alpha,n1,n2,n3,n4,n5,n6,n7,n8,n9};
	Image datenum[6]={d1,d2,d3,d4,d5,d6};

	//1. load templete of the ID card 
	templete.load("template.bmp");	 

	//2. input the information
	cout<<"Your id is: ";  
	scanf("%s",str);
	cout<<"your date (YY/MM/DD): ";
	scanf("%d/%d/%d",&year,&month,&day);
	int symbol[6]={year,year,month,month,day,day};
	cout<<"Select an image as your profile picture: ";	
	scanf("%s",fname);
	profile.load(fname);
	cout<<"Your output file name: ";
	scanf("%s",usrname);
	
	//3. check the id	
	if(check_id(str))	
		cout<<"Your id is valid."<<endl;
	else 	
		cout<<"Your id is invalid!!!!"<<endl;
					
	//4. resize the profile picture 		
	porfile_o=NNI(profile,85,105);
	porfile_o.save("profile.bmp");
				
	//ID sequence dealing
	for(int i=0;i<sizeof(IDnum)/sizeof(IDnum[0]);i++)
	{
		afname[0]=str[i];
		IDnum[i].load(afname);
	}
	
	//gneder dealing
	gname[1]=str[1];
	gender.load(gname);

	//date dealing
	for(int i=0;i<sizeof(datenum)/sizeof(datenum[0]);i++)
	{	
		static int cnt=0;
		if(cnt%2)	afname[0]=symbol[i]%10+48;
		else	afname[0]=symbol[i]/10+48;
		datenum[i].load(afname);
		cnt++;
	}
	
	//5. assemble all the components 		
	porfile(templete,porfile_o,187,59);
	ID(templete,IDnum,187,28);
	date(templete,datenum,77,107,139,47);
	gender_deter(templete,gender,233,45);
	templete.save(usrname);
	
	return 0;
}

