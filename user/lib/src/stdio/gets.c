#include <stdio.h>
#include <unistd.h>

char *gets(char *str) {
    int c;
    int pos = 0;

    while((c = getchar()) != EOF) {
        if(c == '\n' || c == '\r') {
            str[pos] = 0;
            return str;
        }

        str[pos] = (char)c;
        pos++;
    }

    str[pos] = 0;
    
    return NULL;
}
