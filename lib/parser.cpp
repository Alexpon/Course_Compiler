#include<iostream>
#include<cstdlib>
#include<cstring>
#include<cstdio>
#include<string>
#include<fstream>
#include <vector>

using namespace std;
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
int setLLTable();
int is_in_First(string, string);
int is_epsilon(string, string);
void writeLLTable(int);
void simple_Lexical();
int is_keyword(string);
int is_num(string);
void buildTree(int);

struct Tree
{
    int index;       //the depth of the node
    string value;    //the value of the node
};

class Stack         //the stack store type "Tree"
{
    private:
        int top;
        int size;
        Tree *array;
    public:
        Stack(int s)
        {
            size=s;
            array=new Tree[s];
            top=0;
        }

        void push(Tree item)
        {
            if(top==size)
                cout<<"Stack is full!"<<endl;
            else
                *(array+top)=item;
            top++;
        }
        Tree pop()
        {
            if(top==0)
                cout<<"Stack is empty!"<<endl;
            else
            {
                Tree item;
                top--;
                item=*(array+top);
                return item;
            }
            Tree nop;
            return nop; //never reach
        }
};

string grammerMap[100][10]={};      //store grammar.txt into this array
string firstMap[64][32]={};         //store basic first products include some nonterminal(Reverse order)
string finalFirstMap[64][32]={};    //store first products of each nonterminal(In order)
string followRelative[64][2]={};    //store relative between each nonterminal(if A Contains B than store followRelative[n][0]=A && followRelative[n][1]=B)
string followBasic[32][32]={};      //store correct follow products after first scanning, and push all different nonterminal to the top of the same nonterminal
string followMerge[32][32]={};      //according followBasic array, get the first of the same nonterminal and store into this array
string finalFollowMap[64][32]={};   //store follow products of each nonterminal(according followRelative and followMerge array)
string llTableMap[256][10]={};      //store llTable
string mainMap[1024]={};             //store the output of simple lexier(input:main.c)


int grammerRow;                     //the row of grammar
int firstRow;                       //the number of nonterminal

int main(){
    readGrammer();
    scanFirst();
    reSetFirst();
    int rcnt = scanFollow();
    setFollow(rcnt);
    setFollow(rcnt); //second time to make the table more correctly
    writeFile();
    int llrow = setLLTable();
    writeLLTable(llrow);
    simple_Lexical();
    buildTree(llrow);
    return 0;
}

void readGrammer(){

    char line[128];
    memset(line, '\0', sizeof(128));
    fstream fr;
    int col=0;
    int gRow=0, gCol=0;
    fr.open("grammar.txt",ios::in);
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
            while(grammerMap[refRow][0] == "\t" && refRow <96){
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

int scanFollow(){
    int rcnt=0, bcnt=0;
    int row, col;
    int nonTerRow;
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
    int refR1=0, refR2=0;
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

void writeFile(){
    fstream fw;
    fw.open("set.txt",ios::out);
    if(!fw){
        cout << "Fail to open file" << endl;
        exit(1);
    }
    int i;
    fw << "First" << endl;
    for(i=0; i<firstRow; i++){
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
    for(i=0; i<28; i++){
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

int setLLTable(){
    int row, col=1;
    int graRow;
    int llrow=0, llcol=2;
    int graCol=1;
    int graCol2=1;
    for(row=0; row<firstRow; row++){
        while(finalFirstMap[row][col] != "\0"){
            if(finalFirstMap[row][col] == "epsilon"){
                while(finalFollowMap[row][graCol] != "\0"){
                    llTableMap[llrow][0] = finalFollowMap[row][0];
                    llTableMap[llrow][1] = finalFollowMap[row][graCol];
                    graRow = is_epsilon(finalFirstMap[row][0], "epsilon");
                    if(graRow == 0){//expection handling
                        cout << "This is not a regular grammar!" << endl;
                        exit(EXIT_FAILURE);
                    }
                    while(grammerMap[graRow][graCol2] != "\0"){
                        llTableMap[llrow][llcol] = grammerMap[graRow][graCol2];
                        llcol++;
                        graCol2++;
                    }
                    graCol2 = 1;
                    llcol = 2;
                    graCol++;
                    llrow++;
                }
                llrow--;
            }
            else{
                graRow = is_in_First(finalFirstMap[row][0], finalFirstMap[row][col]);
                if(graRow == 0){//expection handling
                    cout << "This is not a regular grammar!" << endl;
                    exit(EXIT_FAILURE);
                }
                llTableMap[llrow][0] = finalFirstMap[row][0];
                llTableMap[llrow][1] = finalFirstMap[row][col];
                while(grammerMap[graRow][graCol] != "\0"){
                    llTableMap[llrow][llcol] = grammerMap[graRow][graCol];
                    llcol++;
                    graCol++;
                }
            }
            llrow++;
            llcol = 2;
            graRow = 1;
            graCol = 1;
            col++;
        }
        col=1;
    }
    return llrow;
}

int is_epsilon(string str, string sym){
    int graRow;
    int tmp;
    int i, j=1;
    for(graRow=0; graRow<96; graRow++){
        if(str == grammerMap[graRow][0]){
            break;
        }
    }
    graRow++;
    tmp = graRow;
    while(grammerMap[graRow][0] == "\t"){
        if(grammerMap[graRow][1] == sym){
            return graRow;
        }
        graRow++;
    }
    graRow = tmp;
    while(grammerMap[graRow][0] == "\t"){
        string tmp = grammerMap[graRow][1];
        if(tmp[0]>=65 && tmp[0]<=90){
            return graRow;
        }
        graRow++;
    }
    return 0;   //non-regular grammar
}

int is_in_First(string str, string sym){
    int graRow;
    int tmp;
    int i, j=1;
    for(graRow=0; graRow<96; graRow++){
        if(str == grammerMap[graRow][0]){
            break;
        }
    }
    graRow++;
    tmp = graRow;
    while(grammerMap[graRow][0] == "\t"){
        if(grammerMap[graRow][1] == sym){
            return graRow;
        }
        graRow++;
    }
    graRow = tmp;
    while(grammerMap[graRow][0] == "\t"){
        for(i=0; i<firstRow; i++){
            if(grammerMap[graRow][1] == finalFirstMap[i][0]){
                break;
            }
        }
        while(finalFirstMap[i][j] != "\0"){
            if(sym == finalFirstMap[i][j]){
                return graRow;
            }
            j++;
        }
        j=0;
        graRow++;
    }
    return 0;   //non-regular grammar
}

void writeLLTable(int llrow){
    fstream fw;
    fw.open("LLtable.txt",ios::out);
    if(!fw){
        cout << "Fail to open file" << endl;
        exit(1);
    }

    int i;
    fw << "S" << endl;

    for(i=0; i<llrow; i++){
        fw << llTableMap[i][0] << "\t";
        if(llTableMap[i][0].size() < 17 && llTableMap[i][0].size() > 7){
            fw << "\t";
        }
        else if(llTableMap[i][0].size() <= 7){
            fw << "\t\t";
        }
        fw << llTableMap[i][1] << "\t\t";

        for(int j=2; j<10; j++){
            fw << llTableMap[i][j] << " ";
        }
        fw << endl;
    }


    fw.close();
}

void simple_Lexical(){
    char line[128];
    int col=0;
    int mainCnt=0;

    memset(line, '\0', sizeof(128));
    fstream fr;
    fr.open("main.c",ios::in);
    if(!fr){
        cout << "Fail to open file" << endl;
        exit(1);
    }
    while(fr.getline(line,sizeof(line),'\n')){
        while(line[col]!='\0'){
            while(line[col]=='\t' || line[col]==' '){   //skip white space ' ' & '/t'
                col++;
            }
            while(line[col]!='\0' && line[col]!=' ' && line[col]!='\t'){
                mainMap[mainCnt] += line[col];
                col++;
            }
            if(is_keyword(mainMap[mainCnt])==1){
            }
            else if(is_num(mainMap[mainCnt])==1){
                mainMap[mainCnt] = "num";
            }
            else{
                mainMap[mainCnt+1] = mainMap[mainCnt];
                mainMap[mainCnt] = "id";
                mainCnt++;
            }
            col++;
            mainCnt++;
        }
        col=0;
        memset(line, '\0', sizeof(line));
    }
    mainMap[mainCnt] = "$";
    fr.close();
}

int is_keyword(string str){
    if(str=="int" || str=="char" || str=="double" || str=="float" || str=="if" || str=="else" || str=="while" || str=="break" ||  str=="for" || str=="print" || str=="return" ||
            str=="{" || str=="}" || str=="[" || str=="]" || str=="(" || str==")" || str==";" || str=="," || str=="+" || str=="-" || str=="*" || str=="/" || str=="=" ||
            str==">" || str=="<" || str==">=" || str=="<=" || str=="!" || str=="!=" || str=="==" || str=="&&" || str=="||"){
        return 1;
    }
    else{
        return 0;
    }
}

int is_num(string str){
    if(str[0]>=48 && str[0]<=57){
        return 1;
    }
    else{
        return 0;
    }
}

void buildTree(int llrow){
    Stack trace(128);
    int mainCnt = 0;
    int llcnt = 2;
    int refR, index;
    Tree tmpTree;
    Tree newNode;
    tmpTree.index=1;
    tmpTree.value="S";
    trace.push(tmpTree);

    fstream fw;
    fw.open("tree.txt",ios::out);
    if(!fw){
        cout << "Fail to open file" << endl;
        exit(1);
    }

    tmpTree=trace.pop();
    while(tmpTree.value != "$"){
        index=tmpTree.index+1;
        if(tmpTree.value == mainMap[mainCnt]){
            for(int i=1; i<tmpTree.index; i++){
                fw << "  ";
            }
            fw << tmpTree.index << " " << tmpTree.value << endl;
            mainCnt++;

            if(mainMap[mainCnt-1]=="id"){
                for(int i=1; i<tmpTree.index+1; i++){
                    fw << "  ";
                }
                fw << tmpTree.index+1 << " " << mainMap[mainCnt] << endl;
                mainCnt++;
            }
        }
        else{
            for(int i=1; i<tmpTree.index; i++){
                fw << "  ";
            }
            fw << tmpTree.index << " " << tmpTree.value << endl;
            for(refR=0; refR<llrow; refR++){
                if(llTableMap[refR][0]==tmpTree.value && llTableMap[refR][1]==mainMap[mainCnt]){
                    break;
                }
            }
            while(llTableMap[refR][llcnt] != "\0"){     //backward push
                llcnt++;
            }

            for(int i=llcnt-1; i>=2; i--){
                newNode.index = index;
                newNode.value = llTableMap[refR][i];
                trace.push(newNode);
            }
            llcnt = 2;
        }
        tmpTree=trace.pop();
    }

    if(tmpTree.value == "$" && mainMap[mainCnt] == "$"){
        fw << "    2 $" << endl;
        cout << "Accept!" << endl;
    }
    else{
        cout << "Reject!" << endl;
    }
}
