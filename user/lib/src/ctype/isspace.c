#include <ctype.h>

int isspace(int c) {
    return (c == ' ')  || (c == '\n') || (c == '\r') ||
           (c == '\f') || (c == '\t') || (c == '\v');
}
