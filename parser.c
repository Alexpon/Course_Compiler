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
void findFollow();

char grammerMap[100][64]={};
char firstMap[32][128]={};
int line;
int firstTableRow;
int firstTableCol;
int firstMapTotalRow;

int main(){
    readFile();
    scanner();
    reSetNonterminal();

    /*int i, j;
    for(i=0; i<32; i++){
        for(j=0; j<128; j++){
            printf("%c", firstMap[i][j]);
        }
        printf("\n");
    }*/

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
    /**
    firstMapTotalRow = firstTableRow;
    for(firstTableRow = firstMapTotalRow-1; firstTableRow>=0; firstTableRow--){
        if(firstMap[firstTableRow][0] == '\t'){
            for(col=0; col<64; col++){

            }
        }
    }**/

    int row, col;
    int i, j;
    int equalRow, firCol=0, temCol;
    int ctmp = 0;
    char tmp[20] = {};
    for(row=0; row<firstTableRow; row++){
        while(firstMap[row][firCol] != ':'){                   //find the first col in ref row
            printf("%c", firstMap[row][firCol]);
            firCol++;
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

                                //setNontermianl(firstTableRow, firstTableCol, equalRow);
                                int refCol=0;
                                while(firstMap[equalRow][refCol] != ':'){                   //find the first col in ref row
                                    refCol++;
                                }
                                refCol++; // !!?
                                while(firstMap[equalRow][refCol] != '\0'){                  //print out the terminal to the end of ref row
                                    //firstMap[nowRow][nowCol] = firstMap[refRow][refCol];
                                    printf("%c", firstMap[equalRow][refCol]);
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
                printf("%c", firstMap[row][col]);
            }
        }
        firCol = 0;
        printf("\n");
    }
}

void findFollow(){

}
