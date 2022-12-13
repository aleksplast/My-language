#include <stdio.h>

#include "translator.h"

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "Rus");

    const char* input = GetComArg(argc, argv);
    FILE* out = fopen("translated.txt", "w");

    Node* node = ReadFromStandart(input);

    TranslateStandart(node, out);

    return 0;
}
