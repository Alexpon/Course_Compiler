#include<iostream>
#include<cstdlib>
#include<cstring>
#include<cstdio>
#include<string>
#include<fstream>

using namespace std;
void readFile();

string mainMap[64][64]={};

int main(){
    readFile();
    for(int i=0; i<24; i++){
        for(int j=0; j<32; j++){
            cout << mainMap[i][j] << " ";
        }
        cout << endl;
    }
}

void readFile(){
    int mainRow = 0;
    int mainCol = 0;
	char line[128];
	memset(line, '\0', sizeof(char) * 128);
	fstream fr;
	int col=0;
	fr.open("main.c",ios::in);
	if(!fr){
		cout << "Fail to open file" << endl;
		exit(1);
	}

	while(fr.getline(line,sizeof(line),'\n')){
		while(line[col]!='\0'){
			if(line[col]==' '){
				col++;
			}
            else if(line[col]=='\t'){
				mainMap[mainRow][mainCol] = "\t";
				mainCol++;
				col++;
			}
			else{
                while(line[col]!='\0' && line[col]!=' ' && line[col]!='\t'){
                    mainMap[mainRow][mainCol] += line[col];
                    col++;
                }
                mainCol++;
			}
		}
		mainRow++;
		mainCol = 0;
		col=0;
		memset(line, '\0', sizeof(char) * 128);
	}
	fr.close();
}
