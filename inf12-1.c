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
    char* code_s = "#include <stdio.h> \n "
                   "int \n"
                   "main () \n"
                   "{\n"
                   "int expr =( ";
    char* code_e = "); \n"
                   "printf (\"%d\", expr); \n"
                   "return 0;\n"
                   "}\n ";
    int fd = open("tmp_file.c", O_WRONLY | O_CREAT | O_TRUNC, 0666);
    write(fd, code_s, strlen(code_s));
    write(fd, expr, strlen(expr));
    write(fd, code_e, strlen(code_e));
    close(fd);
    int pid = fork();
    if (pid == 0) {
        execlp("gcc", "gcc", "tmp_file.c", "-o", "test_file.exe", NULL);
    } else {
        waitpid(pid, NULL, 0);
        execlp("./test_file.exe", "./test_file.exe", NULL);
    }
    return 0;
}
