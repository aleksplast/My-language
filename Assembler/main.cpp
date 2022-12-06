#include <stdio.h>

#include "assembler.h"

int main(int argc, char* argv[])
{
    FILE* logs = fopen("logs.txt", "w");
    fclose(logs);

    const char* input = GetComArg(argc, argv);

    struct asemblr asemblr = {};

    AsmCtor(&asemblr, input);

    AssemblerMain(&asemblr);

    AssemblerMain(&asemblr);

    AsmDetor(&asemblr);

    return 0;
}
