#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void readFile();
void scanner();
void findFirst(char *);
void findFollow();

char grammerMap[100][32]={};
int line;
int main(){
    readFile();
    scanner();
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
    for(row=line-1; row>=0; row--){
        for(col=0; col<32; col++){

            if(grammerMap[row][0] == '\t')
                break;

            tmp[ctmp]=grammerMap[row][col];
            ctmp++;

            if(grammerMap[row][col] == '\0'){
                findFirst(tmp);
                memset(tmp, '\0', ctmp);
                ctmp=0;
                break;
            }
        }
    }
}

void findFirst(char *word){
    printf("%s\n", word);
}

void findFollow(){
}
