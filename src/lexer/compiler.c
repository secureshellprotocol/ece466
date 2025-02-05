#include <unistd.h>

char *name = "jamespiler";

int main(int argc, char *argv[])  {
    
    argv[0] = name;
    if(argc > 1)
        execv("/usr/bin/gcc", argv);
    else
        execv("/usr/bin/gcc", argv);
}
