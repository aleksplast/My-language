#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "assembler.h"
#include "AssemblerDSL.h"

int AsmCtor(struct asemblr* asemblr, const char* input)
{
    const char* mode = "r";
    const char end = '\n';

    asemblr->text = {NULL, 0, 0, NULL};

    if ((asemblr->labels.labelsarray = (int*) calloc(10, sizeof(int))) == NULL)
        return MEMERR;

    asemblr->labels.labelsnum = 10;

    asemblr->header.sign = 'C' + 256 * 'P';
    asemblr->header.version = version;

    TextReader(input, &asemblr->text, mode);

    LinesSeparator(&asemblr->text, '\n');
    asemblr->labels.labelstrarray = (labelsstr*) calloc(30, sizeof(labelsstr));

    LISTING = fopen("listing.txt", "w");

    if ((asemblr->code = (char*) calloc(asemblr->text.nlines * (sizeof(char) * 2 + sizeof(elem_t)) + 3 * sizeof(int), sizeof(char))) == NULL)
        return MEMERR;
}

int AssemblerMain(struct asemblr* asemblr)
{
    int counter = 0;

    asemblr->ip = sizeof(asemblr->header);

    while (counter < asemblr->text.nlines)
    {
        char cmd[10] = "";
        int len = 0;

        if (CheckForComment(asemblr->text.Strings[counter].ptr))
            counter += 1;

        char* line = asemblr->text.Strings[counter].ptr;

        ListingPrint(LISTING, &asemblr->ip, sizeof(int));
        fprintf(LISTING, " | %-10s |", line);

        sscanf(line, "%s%n", cmd, &len);

#define DEF_CMD(name, num, arg, cod)                                                    \
        if (stricmp(cmd, #name) == 0)                                                   \
        {                                                                               \
            CURRENTCOM = CMD_##name;                                                    \
            if  (arg == 1)                                                              \
                GetArg(line + len + 1, asemblr);                                        \
            else                                                                        \
                fprintf(LISTING, " %02X", num);                                         \
            fprintf(LISTING, "\n");                                                     \
            asemblr->ip++;                                                              \
        }                                                                               \
        else
#include "..\Assembler\cmd.h"
#undef DEF_CMD

        if (strchr(cmd, ':'))
        {
            PushLabel(asemblr, cmd);
        }
        counter++;
    }

    asemblr->header.numofel = asemblr->ip - 3 * sizeof(int);

    *(struct asminfo*)(asemblr->code) = asemblr->header;

    FILE* out = fopen("out.txt", "wb");

    if (out == NULL)
        return FILERR;

    fwrite(asemblr->code, sizeof(char), asemblr->ip, out);

    fclose(out);

    return NOERR;
}

bool CheckForComment(char* line)
{
    char* commentptr = NULL;

    if ((commentptr = strchr(line, '#')) != NULL)
    {
        if (commentptr == line)
            return true;
        else
            *commentptr = '\0';
    }
    else
        return false;

    return false;
}

int ListingPrint(FILE* out, void* arg, size_t size)
{
    char* val = (char*) arg;
    for (int i = 0; i < size; i++)
        if (*(val + i) >= 0)
            fprintf(out, "%02X ", *(val + i));
        else
            fprintf(out, "%02X ", (-1) * *(val + i));

    return NOERR;
}

int GetArg(char* arg, struct asemblr* asemblr)
{
    char reg[30] = {};
    elem_t val = 0;
    int len = 0, label = 0;
    char* labelptr = NULL;
    char labelstr[40] = "";

    if ((labelptr = strchr(arg, ':')) != NULL)
    {
        CURRENTCOM |= ARG_IMMED;

        if (sscanf(arg + 1, "%d", &label) == 1)
        {
            GetLabel(asemblr, label);
            asemblr->ip += sizeof(int);
        }
        else if (sscanf(arg + 1, "%s", labelstr) == 1)
        {
            printf("label = %s\n", labelstr);
            GetLabelStr(asemblr, labelstr);
            asemblr->ip += sizeof(int);
        }

        fprintf(LISTING, " %02X ", CURRENTCOM);
        ListingPrint(LISTING, (int*)(asemblr->code + asemblr->ip + 1), sizeof(int));
    }
    else if (*arg == '[')
    {
        GetRamArg(arg, asemblr);
    }
    else if (sscanf(arg, "%lg+%s", &val, reg) == 2)
    {
        CURRENTCOM |= ARG_IMMED;
        *(elem_t*)(asemblr->code + asemblr->ip + 1) = val;

        CURRENTCOM |= ARG_REG;
        asemblr->code[asemblr->ip + sizeof(elem_t) + 1] = CheckReg(reg);

        fprintf(LISTING, " %02X ", CURRENTCOM);
        ListingPrint(LISTING, &val, sizeof(elem_t));
        ListingPrint(LISTING, &(asemblr->code[asemblr->ip + sizeof(elem_t)]), sizeof(char));

        asemblr->ip += sizeof(elem_t) + sizeof(char);
    }
    else if (sscanf(arg, "%lg", &val) == 1)
    {
        CURRENTCOM |= ARG_IMMED;
        *(elem_t*)(asemblr->code + asemblr->ip + 1) = val;

        fprintf(LISTING, " %02X ", CURRENTCOM);
        ListingPrint(LISTING, &val, sizeof(elem_t));

        asemblr->ip += sizeof(elem_t);
    }
    else if (sscanf(arg, "%s", reg) == 1)
    {
        CURRENTCOM |= ARG_REG;
        asemblr->code[asemblr->ip + 1] = CheckReg(reg);

        fprintf(LISTING, " %02X %02X", CURRENTCOM, asemblr->code[asemblr->ip + 1]);

        asemblr->ip += 1;
    }
    else
        return ARGERR;

    return NOERR;
}

int GetLabel(struct asemblr* asemblr, int label)
{
    CheckLabel(asemblr, label);

    *(int*)(asemblr->code + asemblr->ip + 1) = LABELARR[label];

    return NOERR;
}

int GetLabelStr(asemblr* asemblr, char* label)
{
    int counter = 0;
    printf("label = %s\n", label);
    while (asemblr->labels.labelstrarray[counter].name != NULL)
    {
        if (strcmp(asemblr->labels.labelstrarray[counter].name, label) == 0)
        {
            *(int*)(asemblr->code + asemblr->ip + 1) = asemblr->labels.labelstrarray[counter].byte;
            printf("FOUND IT\n");
            return NOERR;
        }
        counter += 1;
    }

    *(int*)(asemblr->code + asemblr->ip + 1) = 0;

    return NOERR;
}

int GetRamArg(char arg[], struct asemblr* asemblr)
{
    char reg[10] = "";
    int len = 0;

    sscanf(arg, "%s%n", reg, &len);

    if (*(arg + len - 1) == ']')
    {
        CURRENTCOM |= ARG_MEM;
        *(arg + len - 1) = ' ';
    }

    GetArg(arg + 1, asemblr);
    *(arg + len - 1) = ']';
}

int CheckLabel(struct asemblr* asemblr, int label)
{
    if (asemblr->labels.labelsnum <= label)
    {
        int* prev = LABELARR;

        int* buffer = (int*) realloc(LABELARR, sizeof(int) * (label * 2));

        if (buffer == NULL)
            return MEMERR;

        LABELARR = buffer;

        for (int i = asemblr->labels.labelsnum; i < label + 2; i++)
            LABELARR[i] = 0;

        if (prev != LABELARR)
            free(prev);

        asemblr->labels.labelsnum = label * 2;

        return NOERR;
    }
    else
        return NOERR;
}

int CheckReg(const char* reg)
{
    int lenght = strlen(reg);

    if (*reg == 'r' && *(reg + 2) == 'x' && lenght == 3)
        if ((int)*(reg + 1) <= 101 && (int) *(reg + 1) >= 97)
            return *(reg + 1) - 'a' + 1;
        else
        {
            printf("Compilation error: unknown register\n");
            return REGERR;
        }
    else
    {
        printf("Compilation error: unknown register\n");
        return REGERR;
    }

    return NOERR;
}

int PushLabel(struct asemblr* asemblr, char* cmd)
{
    int label = 0;
    char* labelstr = (char*) calloc(50, sizeof(char));

    if (sscanf(cmd, "%d", &label) == 1)
    {
        CheckLabel(asemblr, label);

        LABELARR[label] = asemblr->ip - sizeof(asemblr->header);
        fprintf(LISTING, "\n");
        free(labelstr);
    }
    else if (sscanf(cmd, "%[^:]", labelstr) == 1)
    {
        int counter = 0;
        printf("str = %s\n", labelstr);

        while (asemblr->labels.labelstrarray[counter].name != NULL)
        {
            counter++;
        }

        asemblr->labels.labelstrarray[counter].name = labelstr;
        asemblr->labels.labelstrarray[counter].byte = asemblr->ip - sizeof(asemblr->header);
        fprintf(LISTING, "\n");
    }

    return NOERR;
}

const char* GetComArg(int argc, char* argv[])
{
    const char* input = "input.txt";

    if (argc > 1)
        return argv[1];
    else
        return input;
}

int AsmDetor(struct asemblr* asemblr)
{
    free(asemblr->text.Strings);
    free(asemblr->code);
    free(asemblr->labels.labelsarray);
    fclose(LISTING);
    asemblr->code = NULL;
    asemblr->ip = -1;

    return NOERR;
}
