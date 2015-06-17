#include<iostream>
#include<cstdlib>
#include<cstring>
#include<cstdio>
#include<string>
#include<fstream>
#include <sstream>

using namespace std;
void readFile();
void findScope();
void printToFile();
void typeCheck();


string mainMap[64][64]={};
string scopeMap[128][5]={};
int scopeCounter;
int scopeRow;
struct Scope
{
	int count;
	int flag;
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
    scopeCounter = 1;
    readFile();
    findScope();
    typeCheck();
    /*
    for(int i=0; i<24; i++){
        for(int j=0; j<32; j++){
            cout << mainMap[i][j] << " ";
        }
        cout << endl;
    }*/

    for(int i=0; i<scopeRow; i++){
        for(int j=0; j<5; j++){
            cout << scopeMap[i][j] << "\t";
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
	fr.open("test.c",ios::in);
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
    int i, j;
    scopeRow=1;
    Stack scStack(128);
    Scope scope;

    for(i=0; i<64; i++){
        for(j=0; j<64; j++){
            if(mainMap[i][j]=="{"){
                scope.count = scopeCounter;
                scope.flag = 0;
                scStack.push(scope);
                scopeCounter++;
            }
            else if(mainMap[i][j]=="}"){
                if(scStack.getFlag()==0){
                    scopeCounter--;
                }
                scStack.pop();
            }
            else if(mainMap[i][j]=="int" || mainMap[i][j]=="double" || mainMap[i][j]=="void"){
                if(scStack.isEmpty()){
                    scopeMap[scopeRow][0] = "0";
                }
                else{
                    stringstream ss;
                    ss << scStack.getCnt();
                    string s;
                    ss >> s;
                    scopeMap[scopeRow][0] = s;
                    scStack.setFlag();
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
        }
    }
    printToFile();
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

void typeCheck(){
    string targetID, targetType;
    string num, numType;
    string scp;
    int i, j, k;
    int tmpscp, cnt=1;

    Stack tcStack(128);
    Scope tcscope;

    for(i=0; i<64; i++){
        for(j=0; j<64; j++){
            if(mainMap[i][j]=="{"){
                tcscope.count = cnt;
                tcscope.flag = 0;
                tcStack.push(tcscope);
                cnt++;
            }
            else if(mainMap[i][j]=="}"){
                if(tcStack.getFlag()==0){
                    cnt--;
                }
                tcStack.pop();
            }
            else if(mainMap[i][j]=="int" || mainMap[i][j]=="double" || mainMap[i][j]=="void"){
                if(tcStack.isEmpty()){
                        tmpscp = 0;
                    }
                    else{
                        tmpscp = tcStack.getCnt();
                        tcStack.setFlag();
                    }
            }
            else if(mainMap[i][j] == "==" || mainMap[i][j] == "!=" || mainMap[i][j] == ">=" ||
                    mainMap[i][j] == "<=" || mainMap[i][j] == ">" || mainMap[i][j] == "<" || mainMap[i][j] == "="){
                    /*
                    if(tcStack.isEmpty()){
                        scp == "0";
                    }
                    else{
                        stringstream ss;
                        ss << tcStack.getCnt();
                        ss >> scp;
                    }
                    */

                    if(mainMap[i][j-1]=="]"){
                        targetID = mainMap[i][j-4];
                    }
                    else{
                        targetID = mainMap[i][j-1];
                    }
                    numType = "int";
                    num = mainMap[i][j+1];
                    for(k=0; k<num.length(); k++){
                        if(num[k]=='.'){
                            numType = "double";
                        }
                    }
                    for(k=1; k<scopeRow; k++){
                        if(scopeMap[k][1]==targetID && atoi(scopeMap[k][0].c_str())>=tmpscp){
                            targetType = scopeMap[k][2];
                            scp = scopeMap[k][0];
                            break;
                        }
                    }
                    if(targetType != numType){
                        cout << "waring (scope " << scp << ") : " << targetID << " " << targetType << ",\t" << num << "\t" << numType << endl;
                    }
            }
        }
    }
}
