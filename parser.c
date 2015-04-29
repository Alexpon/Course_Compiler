#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void readFile();
void scanner();
void findFirst(char *, int);
void setFirst(char *, int);
void nonTerminal(char *);
void setNontermianl(int, int, int);
void reSetNonterminal();
void finalSetNonterminal();
void findFollow();

char grammerMap[100][64]={};
char firstMap[32][128]={};
char firstMap2[32][128]={};

int line;
int firstTableRow;
int firstTableCol;
int firstMapTotalRow;

int main(){
    readFile();
    scanner();
    reSetNonterminal();
    finalSetNonterminal();
    int i, j;
    for(i=0; i<32; i++){
        for(j=0; j<128; j++){
            printf("%c", firstMap[i][j]);
        }
        printf("\n");
    }

    return 0;
}

/*the end of line is '\0'*/
void readFile(){
    FILE *fpr;
    fpr = fopen("grammer.txt", "r");
    char buf[64];
    int row = 0, col = 0;

    while(fgets(buf, 64, fpr) != NULL){
            for(col=0; col<strlen(buf)-1; col++){
                grammerMap[row][col] = buf[col];
            }
            row++;
    }
    grammerMap[row-1][col] = '|';
    line = row;

}

void scanner(){
    int row, col, ctmp=0;
    char tmp[20]={};
    firstTableRow = 0;
    for(row=line-1; row>=0; row--){
        for(col=0; col<64; col++){

            if(grammerMap[row][0] == '\t')
                break;

            tmp[ctmp]=grammerMap[row][col];
            ctmp++;

            if(grammerMap[row][col+1] == '\0'){
                findFirst(tmp, row);
                memset(tmp, '\0', ctmp);
                ctmp=0;
                break;
            }
        }
    }
    //check again

}

void findFirst(char *word, int row){
    int newline = row+1;
    int col=0;
    char tmp[32]={};
    setFirst(word, 0);//Nonterminal
    while(grammerMap[newline][0]=='\t' && newline<94){
        while(grammerMap[newline][col+1] != '\0' && grammerMap[newline][col+1] != ' '){
                tmp[col] = grammerMap[newline][col+1];
                col++;
        }
        if(tmp[0]>=65 && tmp[0]<=90){
                nonTerminal(tmp);
        }
        else{
            tmp[col] = ' ';
            setFirst(tmp, 1);
        }
            memset(tmp, '\0', col);
            col=0;
            newline++;
    }
    firstTableRow++;
}

void setFirst(char *word, int det){
    int col;
    if(det == 0){
        for(col=0; col<strlen(word); col++){
            firstMap[firstTableRow][col] = word[col];
        }
        firstMap[firstTableRow][col+1] = '\t';
        firstMap[firstTableRow][col+2] = ':';
        firstTableCol = col+3;
    }
    else{
        for(col=0; col<strlen(word); col++){
            firstMap[firstTableRow][firstTableCol+col] = word[col];
        }
        firstTableCol = firstTableCol + col;
    }
}

void nonTerminal(char *state){
    int i, j;
    int equalRow;
    for(i=0; i<firstTableRow; i++){
        equalRow = i;
        for(j=0; j<strlen(state); j++){
            if(state[j]!=firstMap[i][j]){
                equalRow = 100;
            }
        }
        if(firstMap[equalRow][strlen(state)+1] != '\t' && equalRow != 100){
            equalRow = 100;
        }
        if(equalRow!=100){
            setNontermianl(firstTableRow, firstTableCol, equalRow);
            break;
        }
    }
    if(equalRow==100){
        for(i=0; i<strlen(state); i++){
            firstMap[firstTableRow][firstTableCol+i] = state[i];
        }
        firstTableCol+=i;
        firstMap[firstTableRow][firstTableCol] = ' ';
        firstTableCol++;
    }
}

void setNontermianl(int nowRow, int nowCol, int refRow){
    int i;
    int refCol=0;
    while(firstMap[refRow][refCol] != ':'){
        refCol++;
    }
    refCol++;
    while(firstMap[refRow][refCol] != '\0'){
        firstMap[nowRow][nowCol] = firstMap[refRow][refCol];
        nowCol++;
        refCol++;
    }
    firstTableCol=nowCol;
}

void reSetNonterminal(){
    int row, col;
    int row2=0, col2=0;
    int i, j;
    int equalRow, firCol=0, temCol;
    int ctmp = 0;
    char tmp[20] = {};
    for(row=0; row<firstTableRow; row++){
        while(firstMap[row][firCol] != ':'){                   //find the first col in ref row
            firstMap2[row2][col2]=firstMap[row][firCol];
            firCol++;
            col2++;
        }
        for(col=firCol; col<128; col++){     //col=0 always is '\t'
            if(firstMap[row][col]>=65 && firstMap[row][col]<=90){
                temCol = col;
                while(firstMap[row][temCol] != ' ' && firstMap[row][temCol] != '\0'){   //store nonterminal into tmp
                    tmp[ctmp] = firstMap[row][temCol];
                    ctmp++;
                    temCol++;

                    if(firstMap[row][temCol] == ' ' || firstMap[row][temCol] == '\0'){  //end of nonterminal
                        col = temCol;                                              //change col to new col
                        for(i=row; i<line; i++){                                            //find the nonterminal to match tmp
                            equalRow = i;
                            for(j=0; j<strlen(tmp); j++){
                                if(tmp[j]!=firstMap[i][j]){
                                    equalRow = 100;
                                }
                            }
                            if(firstMap[equalRow][strlen(tmp)+1] != '\t' && equalRow != 100){
                                equalRow = 100;
                            }
                            if(equalRow!=100){                                              //already find the match state, row=equalRow
                                int refCol=0;
                                while(firstMap[equalRow][refCol] != ':'){                   //find the first col in ref row
                                    refCol++;
                                }
                                refCol++; // !!?
                                while(firstMap[equalRow][refCol] != '\0'){                  //print out the terminal to the end of ref row
                                    //firstMap[nowRow][nowCol] = firstMap[refRow][refCol];
                                    firstMap2[row2][col2] = firstMap[equalRow][refCol];
                                    col2++;
                                    refCol++;
                                }
                            }
                        }
                        memset(tmp, '\0', ctmp);
                        ctmp=0;
                        break;
                    }
                }
            }
            else{
                firstMap2[row2][col2] = firstMap[row][col];
                col2++;
            }
        }
        firCol = 0;
        row2++;
        col2=0;
    }
}
//almost same as reSetNonterminal
void finalSetNonterminal(){
    int row, col;
    int row2=0, col2=0;
    int i, j;
    int equalRow, firCol=0, temCol;
    int ctmp = 0;
    char tmp[20] = {};
    for(row=0; row<firstTableRow; row++){
        while(firstMap2[row][firCol] != ':'){
            firstMap[row2][col2]=firstMap2[row][firCol];
            firCol++;
            col2++;
        }
        for(col=firCol; col<128; col++){
            if(firstMap2[row][col]>=65 && firstMap2[row][col]<=90){
                temCol = col;
                while(firstMap2[row][temCol] != ' ' && firstMap2[row][temCol] != '\0'){
                    tmp[ctmp] = firstMap2[row][temCol];
                    ctmp++;
                    temCol++;

                    if(firstMap2[row][temCol] == ' ' || firstMap2[row][temCol] == '\0'){
                        col = temCol;
                        for(i=row; i<line; i++){
                            equalRow = i;
                            for(j=0; j<strlen(tmp); j++){
                                if(tmp[j]!=firstMap2[i][j]){
                                    equalRow = 100;
                                }
                            }
                            if(firstMap2[equalRow][strlen(tmp)+1] != '\t' && equalRow != 100){
                                equalRow = 100;
                            }
                            if(equalRow!=100){
                                int refCol=0;
                                while(firstMap2[equalRow][refCol] != ':'){
                                    refCol++;
                                }
                                refCol++; // !!?
                                while(firstMap2[equalRow][refCol] != '\0'){
                                    firstMap[row2][col2] = firstMap2[equalRow][refCol];
                                    col2++;
                                    refCol++;
                                }
                            }
                        }
                        memset(tmp, '\0', ctmp);
                        ctmp=0;
                        break;
                    }
                }
            }
            else{
                firstMap[row2][col2] = firstMap2[row][col];
                col2++;
            }
        }
        firCol = 0;
        row2++;
        col2=0;
    }
}


void findFollow(){

}
