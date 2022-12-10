#include <stdio.h>

#include "processor.h"

int main()
{
    FILE* logs = fopen("logs.txt", "w");
    fclose(logs);

    struct cpu cpu = {};

    CpuCtor(&cpu);

    ProcessorMain(&cpu);

    CpuDetor(&cpu);

    return 0;
}
