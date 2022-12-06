#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <TXLib.h>

#include "processor.h"

#define CPUCHECK if (int errors = CpuVerr(cpu))                                 \
                    DBG CpuDump(*cpu, errors, __LINE__, __func__, __FILE__);

int CpuCtor(struct cpu* cpu)
{
    cpu->commands = {};

    StackCtor(&cpu->commands, 5);
    StackCtor(&cpu->returns, 5);

    FILE* fp = fopen("out.txt", "r");

    long size = FileSize(fp);
    if ((cpu->code = (char*) calloc(size, sizeof(char)))== NULL)
        return MEMERR;

    fread(cpu->code, sizeof(char), size, fp);
    fclose(fp);

    CpuInfoCheck(cpu);

    CPUCHECK

    return NOERR;
}

int CpuInfoCheck(struct cpu* cpu)
{
    cpu->info.sign = *(int*) cpu->code;
    if (cpu->info.sign != 'C' + 256 * 'P')
        return SignErr;

    cpu->info.vers = *(int*) (cpu->code + sizeof(int));
    if (cpu->info.vers != version)
        return VerErr;

    cpu->info.numofel = *(int*) (cpu->code + 2 * sizeof(int));

    cpu->code += 3 * sizeof(int);

    return NOERR;
}

int ProcessorMain(struct cpu* cpu)
{
    txCreateWindow(800, 600);

    for (cpu->ip = 0; cpu->ip < cpu->info.numofel; cpu->ip++)
    {
        char cmd = cpu->code[cpu->ip];

        switch(cmd & ELON)
        {
#define DEF_CMD(name, num, arg, cod)                                    \
case CMD_##name:                                                        \
    cod                                                                 \
    break;
#include "..\Assembler\cmd.h"
#undef DEF_CMD
            default:
                CPUCHECK
                printf("%d UNKNOWN COMMAND\n", *cpu->code);
                return COMERR;
        }
    }
    return NOERR;
}

int CpuVerr(struct cpu* cpu)
{
    int errors = 0;

    assert(cpu != NULL);

    if (cpu->code == NULL)
        errors |= DATAERR;
    if (cpu->ip > cpu->info.numofel || cpu->ip == 0)
        errors |= IPERR;

    errors |= StackErr(&cpu->commands);

    errors |= StackErr(&cpu->returns);

    UpdateHash(&cpu->commands);
    UpdateHash(&cpu->returns);

    return errors;
}

int CpuDump(struct cpu cpu, int errors, int line, const char* func, const char* file)
{
    FILE* logs = fopen("logs.txt", "a");
    fprintf(logs, "\n%s at ", func);
    fprintf(logs, "%s", file);
    fprintf(logs, "(%d)\n", line);
    for (int i = 0; i < cpu.info.numofel; i++)
    {
        fprintf(logs, "%3d ", i);
    }
    fprintf(logs, "\n");
    for(int i = 0; i < cpu.info.numofel; i++)
    {
        fprintf(logs, "%3d ", *(cpu.code + i));
    }
    fprintf(logs, "\n");
    for(int i = 0; i < cpu.ip + 1; i++)
    {
        fprintf(logs, "    ");
    }
    fprintf(logs, " ^\n");
    for(int i = 0; i < cpu.ip + 1; i++)
    {
        fprintf(logs, "    ");
    }
    fprintf(logs, " ip\n");

    fclose(logs);

    errors = StackErr(&cpu.commands);
    StackDump(&cpu.commands, errors, line, func, file);

    errors = StackErr(&cpu.returns);
    StackDump(&cpu.returns, errors, line, func, file);

    UpdateHash(&cpu.commands);
    UpdateHash(&cpu.returns);

    return NOERR;
}

int GetArg(struct cpu* cpu, char cmd)
{
    arg = 0;
    argptr = NULL;

    if (cmd & ARG_IMMED)
    {
        arg += *(elem_t*) (cpu->code + cpu->ip + 1);
        argptr = &arg;
//        printf("arg = %d\n", (int) arg);

        cpu->ip += sizeof(elem_t);
    }
    if (cmd & ARG_REG)
    {
        cpu->ip += 1;

        arg += cpu->regs[(int) cpu->code[cpu->ip]];
        argptr = cpu->regs + cpu->code[cpu->ip];
    }
    if (cmd & ARG_MEM)
    {
        argptr = cpu->ram + (int) arg;
//        printf("pop = %d\n", int(arg));
        arg = cpu->ram[(int)arg];
    }

    return NOERR;
}

int SetLabel(struct cpu* cpu)
{
    int label = *(int*)(cpu->code + cpu->ip + 1);

    cpu->ip = label - 1;

    return NOERR;
}

int RamWrite(struct cpu* cpu)
{
    txSleep(1);
    //txClear();

    for (int i = 0; i < 80 * 60; i++)
    {
        int color = (int) cpu->ram[i];
        txSetColor(RGB(color % 256, (color / 256) % 256, color / 256 / 256));
        txSetFillColor(RGB(color % 256, (color / 256) % 256, color / 256 / 256));
        txRectangle(10 * (i % 80), 10 * (i / 80), 10 * (i % 80 + 1), 10 * (i / 80 + 1));
        txSetFillColor(TX_WHITE);
    }

    return NOERR;
}

int compare(const elem_t a, const elem_t b)
{
    assert(isfinite(a));
    assert(isfinite(b));

    if (fabs(a-b) < EPSILON)
        return 0;
    if ((a-b) > EPSILON)
        return 1;
    if ((a-b) < EPSILON)
        return -1;

    return NOERR;
}

int CpuDetor(struct cpu* cpu)
{
    CPUCHECK

    free(cpu->code);
    StackDetor(&cpu->commands);
    StackDetor(&cpu->returns);
    cpu->ip = 0;

    return NOERR;
}
