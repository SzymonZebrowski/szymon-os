#ifndef UTIL_H
#define UTIL_H

#include "../drivers/screen.h"
#include "../cpu/types.h"

void memory_copy(char *source, char *dest, int n){
    int i=0;
    for(i; i<n; i++){
        *(dest+i) = *(source+i);
    }
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

void introduce(){
    char str[6*80] =        
"\n"                                           
"             _________  __  ______ ___  ____  ____        ____  _____ \n"
"            / ___/_  / / / / / __ `__ \\/ __ \\/ __ \\______/ __ \\/ ___/ \n"
"           (__  ) / /_/ /_/ / / / / / / /_/ / / / /_____/ /_/ (__  ) \n"
"          /____/ /___/\\__, /_/ /_/ /_/\\____/_/ /_/      \\____/____/  \n"
"                     /____/                                             \n\n\0";

    int LINES = 7;


    int i;
    //upper bound
    for(i=0; i<MAX_COLS/2; i++){
        kprint(" ", color_mode(CYAN, WHITE));
        kprint(" ", color_mode(BRIGHT_MAGENTA, WHITE));
    }

    kprint(str, color_mode(BLACK, WHITE));

    //lower bound
    for(i=0; i<MAX_COLS/2; i++){
        kprint(" ", color_mode(CYAN, WHITE));
        kprint(" ", color_mode(BRIGHT_MAGENTA, WHITE));
    }

    //side bounds
    for(i=0; i<(LINES+1)/2; i++){
        kprint_at(" ", 0, 2*i+1, color_mode(BRIGHT_MAGENTA, WHITE));
        kprint_at(" ", 79, 2*i+1, color_mode(CYAN, WHITE));
        
        kprint_at(" ", 0, 2*(i+1), color_mode(CYAN, WHITE));
        kprint_at(" ", 79, 2*(i+1), color_mode(BRIGHT_MAGENTA, WHITE));
    }

}

#endif