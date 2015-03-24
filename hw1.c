#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void init_keyArrray();
int is_operator(char, char, char);
int is_symbol(char);
void store_word(char);
void word_classification();
int is_keyword();
int is_number();

char keyArray[11][8];
char word[128];
int word_count=0;
int new_word=0;

int main(){
        int i=0, j=0;
        int count = 1;
        int buf_length = 0;
        int oper_re = 0;
        int flag = 0;

        init_keyArrray();

        FILE *pf;

        char buf[512];

        pf = fopen("main.c","r");
        while(fgets(buf, 512, pf) != NULL){
                //printf("Line %d\n\t%s : %d\n", count, buf, strlen(buf));
                printf("Line %d\n", count);
                buf_length = strlen(buf);
                while(i<buf_length-1){
                        flag = 0;
                        if(buf[i] == '\t'){
                                i++;
                        }
                        else if(buf[i] == ' '){
                                if(new_word==1)
                                        word_classification();
                                i++;
                        }
                        else{
                                oper_re = is_operator(buf[i], buf[i+1], buf[i+3]);
                                if (oper_re==1){
                                        if(new_word==1)
                                                word_classification();
                                        printf("\tOperators:\t\t%c\n", buf[i]);
                                        i = i+1;
                                        flag = 1;
                                }
                                else if (oper_re==2){
                                        if(new_word==1)
                                                word_classification();
                                        printf("\tOperators:\t\t%c%c\n", buf[i], buf[i+1]);
                                        i = i+2;
                                        flag = 1;
                                }
                                else if (oper_re==3 || oper_re==4){
                                        if(new_word==1)
                                                word_classification();
                                        if(oper_re==3){
                                                printf("\tChar:\t\t\t%c%c%c\n", buf[i], buf[i+1], buf[i+2]);
                                                i = i+3;
                                        }
                                        else{
                                                printf("\tChar:\t\t\t%c%c%c%c\n", buf[i], buf[i+1], buf[i+2], buf[i+3]);
                                                i = i+4;
                                        }
                                        flag = 1;
                                }
                                if (is_symbol(buf[i])==1 && flag==0){
                                        if(new_word==1)
                                                word_classification();
                                        printf("\tSpecial Symbols:\t%c\n", buf[i]);
                                        i = i+1;
                                        flag = 1;
                                }
                                if (flag==0){
                                        store_word(buf[i]);
                                        i=i+1;
                                }
                        }
                }
                i=0;
                count ++;
                memset(buf, '\0', buf_length);
        }
        fclose(pf);

        for(i=0; i<11; i++){
                for(j=0; j<8; j++){
                    printf("%c ", keyArray[i][j]);
                }
                printf("\n");
        }

        return 0;
}

void init_keyArrray(){
        int i, j;
        for(i=0; i<11; i++){
                for(j=0; j<8; j++){
                    keyArray[i][j] = '0';
                }
        }
        keyArray[0][0] = 'b'; keyArray[0][1] = 'r'; keyArray[0][2] = 'e';
        keyArray[0][3] = 'a'; keyArray[0][4] = 'k';
        keyArray[1][0] = 'c'; keyArray[1][1] = 'h'; keyArray[1][2] = 'a';
        keyArray[1][3] = 'r';
        keyArray[2][0] = 'd'; keyArray[2][1] = 'o'; keyArray[2][2] = 'u';
        keyArray[2][3] = 'b'; keyArray[2][4] = 'l'; keyArray[2][5] = 'e';
        keyArray[3][0] = 'e'; keyArray[3][1] = 'l'; keyArray[3][2] = 's';
        keyArray[3][3] = 'e';
        keyArray[4][0] = 'f'; keyArray[4][1] = 'o'; keyArray[4][2] = 'r';
        keyArray[5][0] = 'f'; keyArray[5][1] = 'l'; keyArray[5][2] = 'o';
        keyArray[5][3] = 'a'; keyArray[5][4] = 't';
        keyArray[6][0] = 'i'; keyArray[6][1] = 'f';
        keyArray[7][0] = 'i'; keyArray[7][1] = 'n'; keyArray[7][2] = 't';
        keyArray[8][0] = 'p'; keyArray[8][1] = 'r'; keyArray[8][2] = 'i';
        keyArray[8][3] = 'n'; keyArray[8][4] = 't';
        keyArray[9][0] = 'r'; keyArray[9][1] = 'e'; keyArray[9][2] = 't';
        keyArray[9][3] = 'u'; keyArray[9][4] = 'r'; keyArray[9][5] = 'n';
        keyArray[10][0] = 'w'; keyArray[10][1] = 'h'; keyArray[10][2] = 'i';
        keyArray[10][3] = 'l'; keyArray[10][4] = 'e';

}

int is_operator(char tmp1, char tmp2, char tmp3){
        if((tmp1=='+' || tmp1=='-' || tmp1=='*' || tmp1=='/' || tmp1=='>' || tmp1=='<' || tmp1=='=' || tmp1=='!') && tmp2!='=')
                return 1;
        else if((tmp1=='>' && tmp2=='=') || (tmp1=='<' && tmp2=='=') || (tmp1=='=' && tmp2=='=') || (tmp1=='!' && tmp2=='=') || (tmp1=='&' && tmp2=='&') || (tmp1=='|' && tmp2=='|'))
                return 2;
        else if(tmp1=='\'' && tmp3=='\'')
                return 3;
        else if((tmp2=='\\' && tmp3=='n') || (tmp2=='\\' && tmp3=='t'))
                return 4;
        else
                return 0;
}

int is_symbol(char tmp){
        if(tmp=='[' || tmp==']' || tmp=='(' || tmp==')' || tmp=='{' || tmp=='}' || tmp==';' || tmp==',' )
                return 1;
        else
                return 0;
}

//Store temp string step by step
void store_word(char ch){
        new_word = 1;
        word[word_count] = ch;
        word_count++;
        if(word=='\n' || word=='\t'){
                memset(word, '\0', word_count);
                word_count = 0;
                new_word = 0;
        }
}

//Num Err or Id
void word_classification(){
        if(is_number()==0){
                if(is_keyword()==0){
                        printf("\tIdentifier:\t\t%s\n", word);
                }
        }

        memset(word, '\0', word_count);
        word_count = 0;
        new_word = 0;
}

//Num or Err
int is_number(){
        int x;
        int err_pos = 0;
        if(word[0]=='0' || word[0]=='1' || word[0]=='2' || word[0]=='3' || word[0]=='4' || word[0]=='5' || word[0]=='6' || word[0]=='7' || word[0]=='8' || word[0]=='9'){
                      err_pos = 1;
        }
        for(x=1; x<word_count; x++){
                if(word[x]!='0' && word[x]!='1' && word[x]!='2' && word[x]!='3' && word[x]!='4' && word[x]!='5' && word[x]!='6' && word[x]!='7' && word[x]!='8' && word[x]!='9'){
                        if(err_pos==1){
                                printf("\tError:\t\t\t%s\n", word);
                                return 1;
                        }
                        return 0;
                }
        }
        if(err_pos==1){
                printf("\tNumber:\t\t\t%s\n", word);
                return 1;
        }
        else    return 0;

}

int is_keyword(){
        int x=0, y;
        for(y=0; y<11; y++){
                while(word[x] == keyArray[y][x]){
                            x = x+1;
                }
                if(keyArray[y][x+1]=='0'){
                            printf("\tKeywords:\t\t%s\n", word);
                            return 1;
                }
                x = 0;
        }
        return 0;
}

