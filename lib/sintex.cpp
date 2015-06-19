#include<iostream>
#include<cstdlib>
#include<cstring>
#include<cstdio>
#include<string>
#include<fstream>
#include <sstream>
#include <vector>

using namespace std;
void readFile();
void findScope();
void printToFile();
void readGrammer();
void scanFirst();
void reSetFirst();
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
string findType(int, int, string, string);

string mainMap[64][64]={};
string scopeMap[128][5]={};
string assignTable[64][3]={};
int scopeRow;
int assignRow;
string grammerMap[100][10]={};      //store grammar.txt into this array
string firstMap[64][32]={};         //store basic first products include some nonterminal(Reverse order)
string finalFirstMap[64][32]={};    //store first products of each nonterminal(In order)
string followRelative[64][2]={};    //store relative between each nonterminal(if A Contains B than store followRelative[n][0]=A && followRelative[n][1]=B)
string followBasic[32][32]={};      //store correct follow products after first scanning, and push all different nonterminal to the top of the same nonterminal
string followMerge[32][32]={};      //according followBasic array, get the first of the same nonterminal and store into this array
string finalFollowMap[64][32]={};   //store follow products of each nonterminal(according followRelative and followMerge array)
string llTableMap[256][10]={};      //store llTable
string tmainMap[1024]={};             //store the output of simple lexier(input:main.c)


int grammerRow;                     //the row of grammar
int firstRow;                       //the number of nonterminal




struct Tree
{
    int index;       //the depth of the node
    string value;    //the value of the node
};

struct Scope
{
	int count;
	int flag;
};

class StackT        //the stack store type "Tree"
{
    private:
        int top;
        int size;
        Tree *array;
    public:
        StackT(int s)
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

class Stack
{
	private:
		int top;
		int size;
		Scope *array;
	public:
		Stack(int s){
			size=s;
			array=new Scope[s];
			top=0;
		}

		void push(Scope item){
			if(top==size)
				cout<<"Stack is full!"<<endl;
			else
				*(array+top)=item;
			top++;
		}

		void pop(){
			if(top==0)
				cout<<"Stack is empty!"<<endl;
			else
				top--;
		}

        int isEmpty(){
            if(top==0)
                return 1;
            else
                return 0;
        }

        void setCount(int cnt){
            (*(array+top-1)).count = cnt;
        }

        void setFlag(){
            (*(array+top-1)).flag = 1;
        }

		int getCnt(){
            if(top==0)
				cout<<"Stack is empty!"<<endl;
			else{
			    Scope item;
				item=*(array+top-1);
				return item.count;
			}
		}

		int getFlag(){
            if(top==0)
				cout<<"Stack is empty!"<<endl;
			else{
			    Scope item;
				item=*(array+top-1);
				return item.flag;
			}
		}
};

int main(){
    assignRow = 0;
    readGrammer();
    scanFirst();
    reSetFirst();
    int rcnt = scanFollow();
    setFollow(rcnt);
    setFollow(rcnt); //second time to make the table more correctly
    int llrow = setLLTable();
    writeLLTable(llrow);
    simple_Lexical();
    buildTree(llrow);
    readFile();
    findScope();
    //typeCheck();

    for(int i=0; i<scopeRow; i++){
        for(int j=0; j<5; j++){
            cout << scopeMap[i][j] << "\t";
        }
        cout << endl;
    }

    cout << endl;

    for(int i=0; i<assignRow; i++){
        for(int j=0; j<3; j++){
            cout << assignTable[i][j] << "\t";
        }
        cout << endl;
    }


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

    memset(line, '\0', sizeof(char) * 128);
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
                tmainMap[mainCnt] += line[col];
                col++;
            }
            if(is_keyword(tmainMap[mainCnt])==1){
            }
            else if(is_num(tmainMap[mainCnt])==1){
                tmainMap[mainCnt+1] = tmainMap[mainCnt];
                tmainMap[mainCnt] = "num";
                mainCnt++;
            }
            else{
                tmainMap[mainCnt+1] = tmainMap[mainCnt];
                tmainMap[mainCnt] = "id";
                mainCnt++;
            }
            col++;
            mainCnt++;
        }
        col=0;
        memset(line, '\0', sizeof(line));
    }
    tmainMap[mainCnt] = "$";
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
    StackT trace(128);
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
        if(tmpTree.value == tmainMap[mainCnt]){
            for(int i=1; i<tmpTree.index; i++){
                fw << "  ";
            }
            fw << tmpTree.index << " " << tmpTree.value << endl;
            mainCnt++;

            if(tmainMap[mainCnt-1]=="id"){
                for(int i=1; i<tmpTree.index+1; i++){
                    fw << "  ";
                }
                fw << tmpTree.index+1 << " " << tmainMap[mainCnt] << endl;
                mainCnt++;
            }
            if(tmainMap[mainCnt-1]=="num"){
                for(int i=1; i<tmpTree.index+1; i++){
                    fw << "  ";
                }
                fw << tmpTree.index+1 << " " << tmainMap[mainCnt] << endl;
                mainCnt++;
            }
        }
        else{
            for(int i=1; i<tmpTree.index; i++){
                fw << "  ";
            }
            fw << tmpTree.index << " " << tmpTree.value << endl;
            for(refR=0; refR<llrow; refR++){
                if(llTableMap[refR][0]==tmpTree.value && llTableMap[refR][1]==tmainMap[mainCnt]){
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

    if(tmpTree.value == "$" && tmainMap[mainCnt] == "$"){
        fw << "    2 $" << endl;
        cout << "Accept!" << endl;
    }
    else{
        cout << "Reject!" << endl;
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



void findScope(){
    int i, j, k;
    int scopeCounter=0;
    string targetID, targetType;
    string var, inva, numType, rightType;
    string scp;
    scopeRow=1;
    Stack scStack(128);
    Scope scope;

    for(i=0; i<64; i++){
        for(j=0; j<64; j++){
            if(mainMap[i][j]=="{"){
                scope.count = scopeCounter;
                scope.flag = 0;
                scStack.push(scope);
            }
            else if(mainMap[i][j]=="}"){
                scStack.pop();
            }
            else if(mainMap[i][j]=="int" || mainMap[i][j]=="double" || mainMap[i][j]=="void"){
                if(scStack.isEmpty()){
                    scopeMap[scopeRow][0] = "0";
                }
                else{
                    if(scStack.getFlag()==0){
                        scopeCounter++;
                        scStack.setCount(scopeCounter);
                        scStack.setFlag();
                    }

                    stringstream ss;
                    ss << scStack.getCnt();
                    string s;
                    ss >> s;
                    scopeMap[scopeRow][0] = s;
                }
                scopeMap[scopeRow][1] = mainMap[i][j+1];
                scopeMap[scopeRow][2] = mainMap[i][j];
                if(mainMap[i][j+2]=="["){
                    scopeMap[scopeRow][3] = "true";
                }
                else{
                    scopeMap[scopeRow][3] = "false";
                }
                if(mainMap[i][j+2]=="("){
                    scopeMap[scopeRow][4] = "true";
                }
                else{
                    scopeMap[scopeRow][4] = "false";
                }
                scopeRow++;
            }
            else if(mainMap[i][j] == "==" || mainMap[i][j] == "!=" || mainMap[i][j] == ">=" ||
                    mainMap[i][j] == "<=" || mainMap[i][j] == ">" || mainMap[i][j] == "<" || mainMap[i][j] == "="){
                    int tmp=1;
                    int flag=0;
                    if(mainMap[i][j-1]=="]"){
                        targetID = mainMap[i][j-4];
                    }
                    else{
                        targetID = mainMap[i][j-1];
                    }
                    int equal = 0;
                    if(mainMap[i][j] == "="){
                        equal = 1;
                    }
                    numType = "int";
                    while(mainMap[i][j+tmp] != ";" && mainMap[i][j+tmp] != "{" && mainMap[i][j+tmp] != "\0"){
                        if(flag==0){
                            while(mainMap[i][j+tmp]=="+" || mainMap[i][j+tmp]=="-" || mainMap[i][j+tmp]=="*" || mainMap[i][j+tmp]=="/" || mainMap[i][j+tmp]=="(" || mainMap[i][j+tmp]==")"){
                                tmp++;
                            }
                            var = mainMap[i][j+tmp];
                            flag = 1;
                        }
                        inva = mainMap[i][j+tmp];
                        rightType = findType(equal, scopeCounter, targetID, inva);
                        if(rightType=="double"){
                            numType = "double";
                        }
                        else if(rightType=="operator"){
                            var = "temp";
                        }
                        tmp++;
                    }

                    for(k=scopeRow; k>0; k--){
                        if(scopeMap[k][1]==targetID && atoi(scopeMap[k][0].c_str())<=scopeCounter){
                            targetType = scopeMap[k][2];
                            scp = scopeMap[k][0];
                            break;
                        }
                    }
                    if(targetType != numType){
                        cout << "waring (scope " << scp << ") : " << targetID << " " << targetType << ",\t" << var << "\t" << numType << endl;
                    }
            }
        }
    }
    printToFile();
}

string findType(int eq, int scp, string tar, string var){
    int i,k;
    string tmpNum;
    if(var=="+" || var=="-" ||var=="*" || var=="/"){
        return "operator";
    }
    else if(var=="(" || var==")"){
        return "parentheses";
    }
    else if(var[0]<48 || var[0]>57){
        for(i=assignRow; i>=0; i--){
            if(assignTable[i][1]==var && atoi(assignTable[i][0].c_str())<=scp){
                tmpNum = assignTable[i][2];
                for(k=0; k<tmpNum.length(); k++){
                    if(var[k]=='.'){
                        return "double";
                    }
                }
                return "int";
            }
        }
    }
    else{
        stringstream ss;
        ss << scp;
        string s;
        ss >> s;
        for(k=0; k<var.length(); k++){
            if(var[k]=='.'){
                if(eq){
                    assignTable[assignRow][0] = s;
                    assignTable[assignRow][1] = tar;
                    assignTable[assignRow][2] = var;
                    assignRow++;
                }
                return "double";
            }
        }
        if(eq){
            assignTable[assignRow][0] = s;
            assignTable[assignRow][1] = tar;
            assignTable[assignRow][2] = var;
            assignRow++;
        }
        return "int";
    }
}

void printToFile(){
    scopeMap[0][0] = "Scope";
    scopeMap[0][1] = "Symbol";
    scopeMap[0][2] = "Type";
    scopeMap[0][3] = "Array";
    scopeMap[0][4] = "Function";
    fstream fw;
    fw.open("Symbol table.txt",ios::out);
	if(!fw){
		cout << "Fail to open file" << endl;
		exit(1);
	}
    for(int i=0; i<scopeRow; i++){
        for(int j=0; j<5; j++){
            fw << scopeMap[i][j] << "\t";
        }
        fw << endl;
    }
}

