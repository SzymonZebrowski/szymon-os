#include "string.h"

int strlen(char s[]){
    int i=0;
    while(s[i]!='\0') i++;
    return i;
}
void int_to_str(int n, char str[]){
    int i=0;
    int is_negative = n < 0 ? 1 : 0;
    if(is_negative) n=-n;

    do{
        str[i++] = n%10 + '0';
    } while(n/=10);

    
    if(is_negative) str[i++] = '-';

    i-=1;

    int j=0;
    for(j; j<(i+1)/2;j++){
            char t=str[j];
            str[j] = str[i-j];
            str[i-j] = t;
    }
    
    str[++i]='\0';

}

void int_to_hex_str(int n, char str[]){
    int i=0;
    int pom;

    do{
        pom = n%16;
        if(pom < 10) str[i++]= '0' + pom;
        else str[i++]='A' + pom - 10;
    } while(n/=16);
    
    i-=1;

    int j=0;
    for(j; j<(i+1)/2;j++){
            char t=str[j];
            str[j] = str[i-j];
            str[i-j] = t;
    }
    
    str[++i]='\0';
}

void byte_converter(int n, char str[]){
    int i=0;
    int pom;

    
    pom = n%16;
    if(pom < 10) str[i++]= '0' + pom;
    else str[i++]='A' + pom - 10;
    n/=16;
    pom = n%16;
    if(pom < 10) str[i++]= '0' + pom;
    else str[i++]='A' + pom - 10;
    
    char p = str[0];
    str[0]=str[1];
    str[1]=p;
    
    str[2]='\0';
}

void reverse(char str[]){
    int i = strlen(str);
    int j=0;
    for(j; j<(i+1)/2;j++){
            char t=str[j];
            str[j] = str[i-j];
            str[i-j] = t;
    }
}

void backspace(char s[]){
    int len = strlen(s);
    s[len-1] = '\0';
}
void append(char s[], char n){
    int len = strlen(s);
    s[len] = n;
    s[len+1]='\0';
}
int strcmp(char s1[], char s2[]){
    int i;
    for(i=0; s1[i]==s2[i]; i++){
        if(s1[i] == '\0') return 0;
    }
    return s1[i]-s2[i];
}