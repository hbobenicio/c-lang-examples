#include <stdio.h>
#include <string.h>

#define MsgInLen 4
#define MsgOutLen 3
#define MsgExecLen 3

int main() {
    char  dyn_var1[MsgInLen-1] = "a";
    char dyn_var2[MsgOutLen-1] = "1";

    printf("antes da copia:\n");
    printf("dyn_var1: %s\n", dyn_var1);
    printf("dyn_var2: %s\n", dyn_var2);
    
    printf("\nstrncpy(dyn_var2 /*size: 2*/, \"xy\", MsgExecLen /*size: 3*/)\n");
    strncpy(dyn_var2, "xy", MsgExecLen);

    printf("\ndepois da copia:\n");
    printf("(!) dyn_var1: %s\n", dyn_var1);
    printf("dyn_var2: %s\n", dyn_var2);
}

