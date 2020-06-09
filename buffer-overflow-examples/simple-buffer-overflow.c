#include <stdio.h>
#include <string.h>

/**
A stack no kernel linux cresce no sentido inverso ao endereçamento:
https://www.cs.miami.edu/home/burt/learning/Csc421.171/workbook/stack-memory.html

 0xF0  0xF1  0xF2  0xF3   0xF4  0xF5  0xF6  0xF7  0xF8  0xF9  0xFA  0xFB   0xFC
['h'] ['u'] ['g'] ['o'] ['\0'] ['b'] ['e'] ['n'] ['i'] ['c'] ['i'] ['o'] ['\0']
  |                              |
b-+                            a-+

strcpy(b, a);

 0xF0  0xF1  0xF2  0xF3  0xF4  0xF5  0xF6   0xF7  0xF8  0xF9  0xFA  0xFB   0xFC
['b'] ['e'] ['n'] ['i'] ['c'] ['i'] ['o'] ['\0'] ['i'] ['c'] ['i'] ['o'] ['\0']
  |                             |
b-+                           a-+

*/
int main() {
    char a[8] = "benicio";
    char b[5] = "hugo";

    puts("antes da cópia:");
    printf("a = \"%s\" (sizeof(a) = %lu / strlen(a) = %lu)\n", a, sizeof(a), strlen(a));
    printf("b = \"%s\" (sizeof(b) = %lu / strlen(b) = %lu)\n", b, sizeof(b), strlen(b));

    puts("\ncopiando a em cima de b (com overflow)...");
    strcpy(b, a);

    puts("\ndepois da cópia:");
    printf("a = \"%s\" (sizeof(a) = %lu / strlen(a) = %lu) <- efeito colateral!\n", a, sizeof(a), strlen(a));
    printf("b = \"%s\" (sizeof(b) = %lu / strlen(b) = %lu) <- overflow!\n", b, sizeof(b), strlen(b));
}

