#include <stdio.h>

#include "mylang.h"

int main(int argc, char* argv[])
{
    const char* input = GetComArg(argc, argv);

    LanguageMain(input);

    return 0;
}
