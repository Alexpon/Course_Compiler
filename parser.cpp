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
void scanFirst();
void reSetFirst();
void writeFile();
int scanFollow();
void setFollowRelative(int, int, int);
int setFollowBasic(int, int, int, int);
void mergeFollowBasic(int);
void storeMerge(int);
void setFollow(int);


string grammerMap[96][10]={};
string firstMap[64][32]={};
string finalFirstMap[64][32]={};
string followRelative[64][2]={};
string followBasic[32][32]={};
string followMerge[32][32]={};
string finalFollowMap[64][32]={};
int grammerRow;
int firstRow;

int main(){
    init();
    readGrammer();
    scanFirst();
    reSetFirst();
    int rcnt = scanFollow();
    setFollow(rcnt);
    setFollow(rcnt); //second time to make the table more correctly
    writeFile();
    return 0;
}

void init(){
    int i, j;
    for(i=0; i<96; i++){
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
    memset(line, '\0', sizeof(128));
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

void scanFirst(){
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

void writeFile(){
    fstream fw;
    fw.open("set.txt",ios::out);
    if(!fw){
        cout << "Fail to open file" << endl;
        exit(1);
    }
    int i, j;
    fw << "First" << endl;
    for(i=1; i<firstRow; i++){
        fw << finalFirstMap[i][0] << "\t";
        if(finalFirstMap[i][0].size() < 17 && finalFirstMap[i][0].size() > 7){
            fw << "\t";
        }
        else if(finalFirstMap[i][0].size() <= 7){
            fw << "\t\t";
        }
        fw << ": ";
        for(int j=1; j<32; j++){
            fw << finalFirstMap[i][j] << " ";
        }
        fw << endl;
    }

    fw << "\n\n\n";
    fw << "Follow" << endl;
    for(i=1; i<28; i++){
        fw << finalFollowMap[i][0] << "\t";
        if(finalFollowMap[i][0].size() < 17 && finalFollowMap[i][0].size() > 7){
            fw << "\t";
        }
        else if(finalFollowMap[i][0].size() <= 7){
            fw << "\t\t";
        }
        fw << ": ";
        for(int j=1; j<32; j++){
            fw << finalFollowMap[i][j] << " ";
        }
        fw << endl;
    }

    fw.close();
}

int scanFollow(){
    int rcnt=0, bcnt=0;
    int row, col;
    int nonTerRow;
    int i, flag=0;
    for(nonTerRow=0; nonTerRow<firstRow; nonTerRow++){
        for(row=0; row<95; row++){
            for(col=0; col<10; col++){
                if(finalFirstMap[nonTerRow][0] == grammerMap[row][col]){
                    if(col==0){/*do nothing*/}
                    else if(grammerMap[row][col+1] == "\0"){
                        setFollowRelative(row, col, rcnt);
                        rcnt++;
                    }
                    else{
                        if(setFollowBasic(row, col, bcnt, rcnt)==1){
                            rcnt++;
                        }
                        bcnt++;
                    }
                }
            }
        }
    }
    mergeFollowBasic(bcnt);
    return rcnt;
}

void setFollowRelative(int row, int col, int cnt){
    followRelative[cnt][0] = grammerMap[row][col];
    while(grammerMap[row][0]=="\t"){
        row--;
    }
    followRelative[cnt][1] = grammerMap[row][0];
}

int setFollowBasic(int row, int col, int cnt, int rcnt){
    int i;
    int tmpCol=1;
    int isEps=0;
    followBasic[cnt][0] = grammerMap[row][col];
    followBasic[cnt][1] = grammerMap[row][col+1];
    for(i=0; i<firstRow; i++){
        if(grammerMap[row][col+1] == finalFirstMap[i][0]){
            while(finalFirstMap[i][tmpCol] != "\0"){
                if(finalFirstMap[i][tmpCol] == "epsilon"){
                    followRelative[rcnt][0] = grammerMap[row][col];
                    followRelative[rcnt][1] = finalFirstMap[i][0];
                    isEps=1;
                    tmpCol++;
                }
                else{
                    followBasic[cnt][tmpCol] = finalFirstMap[i][tmpCol];
                    tmpCol++;
                }
            }
            break;
        }
    }
    if(isEps==1){
        return 1;
    }
    else{
        return 0;
    }
}

void mergeFollowBasic(int cnt){
    int row, col;
    int i;
    int tmpRow=1, tmpCol=1;
    int tmpCnt=1;
    int equalFlag=0;
    for(i=0; i<cnt; i++){
        while(followBasic[i][0]==followBasic[i+tmpRow][0]){
            while(followBasic[i+tmpRow][tmpCol] != "\0"){
                while(followBasic[i][tmpCnt] != "\0"){
                    if(followBasic[i+tmpRow][tmpCol]==followBasic[i][tmpCnt]){
                        equalFlag=1;
                    }
                    tmpCnt++;
                }
                if(equalFlag==0){
                    followBasic[i][tmpCnt] = followBasic[i+tmpRow][tmpCol];
                }
                equalFlag=0;
                tmpCol++;
                tmpCnt=1;
            }
            tmpRow++;
            tmpCol=1;
        }
        i=i+tmpRow-1;
        tmpRow=1;
    }
    storeMerge(cnt);
}

void storeMerge(int cnt){
    int i, j=0;
    int row=0;
    for(i=0; i<cnt; i++){
        if(followBasic[i][0]!=followBasic[i-1][0]){
            while(followBasic[i][j] != "\0"){
                followMerge[row][j] = followBasic[i][j];
                j++;
            }
            row++;
            j=0;
        }
    }
}

void setFollow(int relativeRow){
    int rcnt, ccnt=1;
    int i, j=1;
    int frow;
    int refR1, refR2;
    int refC1=1, refC2=1;
    int equalFlag=0;
    for(rcnt=0; rcnt<firstRow; rcnt++){
        finalFollowMap[rcnt][0] = finalFirstMap[rcnt][0];
        for(i=0; i<32; i++){
            if(finalFollowMap[rcnt][0] == followMerge[i][0]){
                while(followMerge[i][j] != "\0"){
                    finalFollowMap[rcnt][ccnt] = followMerge[i][j];
                    j++;
                    ccnt++;
                }
                break;
            }
        }
        ccnt=1;
        j=1;
    }
    for(i=0; i<relativeRow; i++){
        if(followRelative[i][0] != followRelative[i][1]){
            //find reference row in final follow map
            for(frow=0; frow<32; frow++){
                if(followRelative[i][0] == finalFollowMap[frow][0]){
                    refR1 = frow;
                }
                if(followRelative[i][1] == finalFollowMap[frow][0]){
                    refR2 = frow;
                }
            }

             while(finalFollowMap[refR2][refC2] != "\0"){
                while(finalFollowMap[refR1][refC1] != "\0"){
                    if(finalFollowMap[refR1][refC1] == finalFollowMap[refR2][refC2]){
                        equalFlag=1;
                    }
                    refC1++;
                }
                if(equalFlag==0){
                    finalFollowMap[refR1][refC1] = finalFollowMap[refR2][refC2];
                }
                equalFlag=0;
                refC2++;
                refC1 = 1;
            }
            refC2 = 1;
        }
        else{/*do nothing*/}
    }

}
