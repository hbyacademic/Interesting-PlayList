/********************************************************************************
 - This program is the simple implementation of the LZW lossless compression
 	1. input format: text file (e.g. xxx.txt)
	2. prvoide a complete encoder and decoder
	3. if you have any questions or suggestions, welcome to discuss with me
		  Email: rickyrita520@gmail.com
	
 - for compression performance, it only can store 128 elements in dictionary (0~127)
	- NOT SUITABLE for large file 								(-1 represents "\n")

 - for correct performance, suggest not to change the following settings
 	- vector<char>encoded_str (V) ; vector<unsigned char>encoded_str (X)
 	- -1 represents "\n" (V) ; 255 represents "\n" (X)
 		 	
 - Written by Bo-Yuan Huang, 
 				the department of CSIE, 
					National Chi Nan University, 
						Puli, Nantou, Taiwan 
																        2017/12/01
*********************************************************************************/

#include<iostream>
#include<string>
#include<vector>
#include<iomanip>
#include<fstream>
using namespace std;

vector<string>dict;	//original dictionary
vector<string>dec_dict;	//decode dictionary
vector<char>encoded_str;	//char -128~127 ; unsigned char 0~255 
vector<string>decoded_str;
int org_dict_content_amt=0;	

void org_dict_content(fstream &input)
{
	string instr;
	while(getline(input,instr))
	{
		for(int i=0;i<instr.length();i++)
		{
			bool not_exist=true;
			string check_instr=instr.substr(i,1);
			for(int j=0;j<dict.size();j++)
			{
				if(dict[j]==check_instr)
				{
					not_exist=false;
					break;
				}
			}
			if(not_exist)	//if the current character didn't exist in the dictionary, then put into it.
			{
				dict.push_back(check_instr);
				org_dict_content_amt++;	//record the amount of the original dictionary content
			}
		}
	}
	input.close();
}

void encoder(fstream &input,char *filename)
{	
	string str,instr=""; int step=1,linecount=0;
	ifstream fin;
	fin.open(filename,ios::in);
    fin.seekg(0, ios::end);	//seek from the beginning to the end of the input file
    long long in_file_size=fin.tellg();	//tell the position, i.e. the size of the input file
    fin.close();
	//cout<<"The size of the input file is "<<in_file_size<<"bytes."<<endl;
	
	while(getline(input,str))	//concatenate all the string in the input file
	{
		instr+=str;
		instr+="@"; 	//"@" represent "\n"
		linecount++;	//the amount of the line in the input file
	}
	//every two line contains "\r" and "\n", in_file_size must minus (linecount-1) 
	if(instr.length()-(in_file_size-(linecount-1))==1)	//the end of the input file without "\n"
		instr=instr.substr(0,instr.length()-1);	//erase the last character ("@") of the instr
	input.close();
	
	int ctr,reduce=0,j,match=1,pos;
	for(int i=0;i<instr.length();i++)
	{							
		pos=instr.length()-i;	//the longest possible common subsequence
		while(match)	//if match, then we don't try to match other content with the shorter one (e.g. match BB, then not to match B twice)
		{
			if(reduce) pos--;	//if didn't match all the content, then shorten the common subsequence and find it again
			for(j=dict.size()-1;j>=0;j--)	
			{
				if(dict[j]==instr.substr(i,pos))	//match!!!
				{
					encoded_str.push_back(j+1);	//record the encode result => to form the encoded input string
					cout<<setw(4)<<step;
					
					if(i+pos==instr.length())	//match the common subsequence to the end of the input string
					{
						if(instr[i+pos]=='@')	// the next character is "\n" 
						{	
							encoded_str.push_back(-1); 
							cout<<setw(8)<<instr.substr(i,pos)<<","<<setw(4)<<j+1<<endl;
							i+=(pos-1);		//update the starting point for finding the next common subsequence
						}
						else
						{
							cout<<setw(8)<<instr.substr(i,pos+1)<<", "<<setw(4)<<j+1<<endl;
							i+=(pos-1);	
						}
					}

					else if(instr[i+pos]=='@')	// "@" represent "\n" => -1 for decoder flag => will generate "\n"
					{
						ctr=0; 
						while(instr[i+pos+ctr]=='@')	//meet the "\n" continuously
						{
							encoded_str.push_back(-1);	
							ctr++;	
						}
						dict.push_back(instr.substr(i,pos)+instr.substr(i+pos+ctr,1));	
						cout<<setw(8)<<instr.substr(i,pos)<<","<<instr.substr(i+pos+ctr,1);	
						cout<<setw(4)<<j+1<<setw(8)<<dict.size()<<setw(12)<<instr.substr(i,pos)+instr.substr(i+pos+ctr,1)<<endl;
						i+=(pos+ctr-1);	
					}
			
					else	
					{	//concatenate the common subsequence with the next character, and then store it in the dictionary
						dict.push_back(instr.substr(i,pos+1));	
						cout<<setw(8)<<instr.substr(i,pos)<<","<<instr.substr(i+pos,1);
						cout<<setw(4)<<j+1<<setw(8)<<dict.size()<<setw(12)<<instr.substr(i,pos+1)<<endl;
						i+=(pos-1);		//because in the for-loop i will ++, i+=(pos-1) not i+=pos (it will update to the wrong starting point)
					}
					match=0;
					step++;
					break;
				}
			}
			(j==-1)?reduce=1:reduce=0;	//determine match or not
		}
		match=1;	//reset
		reduce=0;
	}
}

void decoder()
{
	for(int i=0;i<org_dict_content_amt;i++)	//copy the original dict content to the dec_dict
		dec_dict.push_back(dict[i]);
		
	string instr;
	int step=2;
	
	cout<<setw(4)<<"1"<<setw(10)<<encoded_str[0]<<setw(9)<<dec_dict[encoded_str[0]-1]<<endl;	//decode the first character of the encoded string
	decoded_str.push_back(dec_dict[encoded_str[0]-1]);	//record the decode result => to form the decoded input string
		
	string current,prev=dec_dict[encoded_str[0]-1],dec_content;
		
	for(int i=1;i<encoded_str.size();i++)
	{
		if(encoded_str[i]==-1)	// -1 represent "\n"
		{
			decoded_str.push_back("\n");
			continue;
		} 
		
		else if(encoded_str[i]<=dec_dict.size())	//if can find it in the decode dictionary
		{
			current=dec_dict[encoded_str[i]-1];
			cout<<setw(4)<<step<<setw(10)<<encoded_str[i]<<setw(9)<<dec_dict[encoded_str[i]-1]<<setw(6)<<prev<<","<<current;		
			decoded_str.push_back(dec_dict[encoded_str[i]-1]);	//record the decode result => to form the decoded input string
		}
		else	// if can't find it, then the current output will be the string that prev concatenate with prev[0]
		{
			current=prev+prev[0];
			cout<<setw(4)<<step<<setw(10)<<encoded_str[i]<<setw(9)<<prev+prev[0]<<setw(6)<<prev<<","<<current;
			decoded_str.push_back(prev+prev[0]);	//record the decode result => to form the decoded input string
		}
		dec_content=prev+current.substr(0,1);	//every two output result will do specific concatenate (prev+current[0])
		dec_dict.push_back(dec_content);		//and then put into the decode dictionary
		cout<<setw(7-(current.length()-2))<<dec_dict.size()<<setw(12)<<dec_content<<endl;
		prev=current;	//put the current one to the prev one, so the current one can record the new decoding result
		step++;
	}
}

int main()
{
	fstream input,output,en_result; 
	string str;	
	int group=0;
	char filename[80];
	cout<<"The input filename: "; cin>>filename;
	cout<<"--------------------------------------------------------------------"<<endl;
	
	cout<<"\nThe content of the input file: \n";
	input.open(filename,ios::in);
	while(getline(input,str))
		cout<<str<<endl;
	input.close();
	cout<<"--------------------------------------------------------------------"<<endl;
	input.open(filename,ios::in);
	org_dict_content(input);
	cout<<"The oroginal dictionary content: ";
	for(int i=0;i<dict.size();i++)
		cout<<dict[i]<<" ";
	
	cout<<"\n\n--------------The details of the encryption processing--------------"<<endl;	
	cout<<"編碼步驟  (S,C) 輸出  字典索引   字典內容 "<<endl;
	for(int i=0;i<dict.size();i++)
		cout<<setw(4)<<"0"<<setw(22)<<i+1<<setw(12)<<dict[i]<<endl;
	
	input.open(filename,ios::in);
	encoder(input,filename);
	cout<<"\n=> The encoded content of input file : (-1 represents \"\\n\")"<<endl;
	
	en_result.open("encoded.txt",ios::out);	//write the decoding result into the text file
	//the compression concern: endl, setw(5)," " can be removed
	for(int i=0;i<encoded_str.size();i++)
	{
		if(group==10)	//ten elements in each line
		{
			group=0;
			cout<<endl;
			//en_result<<endl;	
		} 
		cout<<setw(5)<<int(encoded_str[i])<<" ";
		//en_result<<setw(5)<<encoded_str[i]<<" ";
		en_result<<encoded_str[i];
		group++;
	}	
	cout<<endl;
	group=0;
		
	cout<<"\n\n--------------The details of the decryption processing--------------"<<endl;
	cout<<"解碼步驟  解碼索引  輸出  (S1,S2)  字典索引   字典內容 "<<endl;
	for(int i=0;i<org_dict_content_amt;i++)
		cout<<setw(4)<<"0"<<setw(35)<<i+1<<setw(12)<<dict[i]<<endl;
	
	decoder();
	cout<<"\n=> The result of encoded file : \n";
	for(int i=0;i<decoded_str.size();i++)
		cout<<decoded_str[i];
	
	output.open("output.txt",ios::out);	//write the decoding result into the text file
	for(int i=0;i<decoded_str.size();i++)
		output<<decoded_str[i];
	output.close();
	cout<<endl;
	system("Pause");
} 
