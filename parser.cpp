#include<iostream>
#include<cstdlib>
#include<cstring>
#include<cstdio>
#include<string>
#include<fstream>
#include <vector>

using namespace std;
void init();
void readGrammer();
void scanner();
void reSetFirst();
void writeFirst();

string grammerMap[95][10]={};
string firstMap[64][32]={};
string finalFirstMap[64][32]={};
int grammerRow;
int firstRow;

int main(){
    init();
    readGrammer();
    scanner();
    reSetFirst();
    writeFirst();
    int i, j;
    return 0;
}

void init(){
    int i, j;
    for(i=0; i<95; i++){
        for(int j=0; j<10; j++){
            grammerMap[i][j]="\0";
        }
    }
    for(i=0; i<64; i++){
        for(int j=0; j<32; j++){
            firstMap[i][j]="\0";
            finalFirstMap[i][j]="\0";
        }
    }
}

void readGrammer(){

    char line[128];
    fstream fr;
    int row=0, col=0;
    int gRow=0, gCol=0;
    fr.open("grammer.txt",ios::in);
    if(!fr){
        cout << "Fail to open file" << endl;
        exit(1);
    }

    while(fr.getline(line,sizeof(line),'\n')){
            while(line[col]!='\0'){
                if(line[col]=='\t'){
                    grammerMap[gRow][gCol]="\t";
                    gCol++;
                    col++;
                }
                while(line[col]!='\0'&&line[col]!=' '){
                    grammerMap[gRow][gCol] += line[col];
                    col++;
                }
                col++;
                gCol++;
            }
            col=0;
            gRow++;
            gCol=0;
            memset(line, '\0', sizeof(line));
    }
    grammerRow = gRow;
    fr.close();
}

void scanner(){
    int row;
    int refRow;
    int nonRow, nonCol=1;
    int frow=0, fcol=0;
    string str;
    for(row=grammerRow-1; row>=0; row--){
            if(grammerMap[row][0] != "\t"){
                firstMap[frow][fcol] = grammerMap[row][0];
                fcol++;
                refRow = row+1;
                while(grammerMap[refRow][0] == "\t" && refRow <94){
                    str = grammerMap[refRow][1];
                    if(str[0]>=65 && str[0]<=90){
                        firstMap[frow][fcol]=str;
                        fcol++;
                        for(nonRow=frow; nonRow>=0; nonRow--){
                            if(str==firstMap[nonRow][0]){
                                fcol--;
                                while(firstMap[nonRow][nonCol]!="\0"){
                                    firstMap[frow][fcol] = firstMap[nonRow][nonCol];
                                    nonCol++;
                                    fcol++;
                                }
                                nonCol=1;
                            }
                        }
                        refRow++;
                    }
                    else{
                        firstMap[frow][fcol] = grammerMap[refRow][1];
                        fcol++;
                        refRow++;
                    }
                }
                frow++;
                fcol = 0;
            }
    }
    firstRow=frow;
}

void reSetFirst(){
    int row, col;
    int frow=0, fcol=1;
    int nonRow, nonCol=1;
    string str;
    for(row=firstRow-1; row>=0; row--){
        finalFirstMap[frow][0] = firstMap[row][0];
        for(col=1; col<32; col++){
            str = firstMap[row][col];
            if(str[0]>=65 && str[0]<=90){
                for(nonRow=frow; nonRow>=0; nonRow--){
                    if(str==finalFirstMap[nonRow][0]){
                        while(finalFirstMap[nonRow][nonCol]!="\0"){
                            finalFirstMap[frow][fcol] = finalFirstMap[nonRow][nonCol];
                            nonCol++;
                            fcol++;
                        }
                        nonCol=1;
                    }
                }
            }
            else{
                finalFirstMap[frow][fcol] = firstMap[row][col];
                fcol++;
            }
        }
        fcol=1;
        frow++;
    }
}

void writeFirst(){
    fstream fw;
    fw.open("set.txt",ios::out);
    if(!fw){
        cout << "Fail to open file" << endl;
        exit(1);
    }
    int i, j;
    for(i=0; i<firstRow; i++){
        fw << finalFirstMap[i][0] << "\t: ";
        for(int j=1; j<32; j++){
            fw << finalFirstMap[i][j] << " ";
        }
        fw << endl;
    }
    fw.close();
}
