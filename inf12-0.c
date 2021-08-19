#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    char expr[4096];
    fgets(expr, sizeof(expr), stdin);
    char* code_s = "res = (";
    char* code_e = "); print(res)";
    char formula[8192];
    snprintf(formula, sizeof(formula), "%s%s%s", code_s, expr, code_e);
    execlp("python3", "python3", "-c", formula, NULL);
    return 0;
}
