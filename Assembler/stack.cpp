#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>

#include "Config.h"
#include "stack.h"

const double RESIZECOEFF = 2.0;
const double SHRINKCOEFF = 0.375;

#define DataInit    stk->dataguardl = (unsigned long long*) buffer;                                                                 \
                    stk->dataguardl[0] = CANARY;                                                                                    \
                    stk->data = (elem_t*) (buffer + sizeof(unsigned long long));                                                    \
                    stk->dataguardr = (unsigned long long*) (buffer + sizeof(unsigned long long) + stk->capacity * sizeof(elem_t)); \
                    stk->dataguardr[0] = CANARY;

int StackCtor(struct stack* stk, size_t capacity)
{
    DBG assert(stk != NULL);

    char* buffer = (char*) calloc(1, capacity * sizeof(elem_t) + 2 * sizeof(unsigned long long));

    if (buffer == NULL)
        return MEMERR;

    stk->capacity = capacity;

    DataInit

    stk->canaryleft = CANARY;
    stk->size = 0;
    stk->canaryright = CANARY;

    FillWPoison(stk, 0, (int)stk->capacity);

    UpdateHash(stk);

    if (int errors = StackErr(stk))
        DBG StackDump(stk, errors, __LINE__, __func__, __FILE__);

    UpdateHash(stk);

    return NOERR;
}

int FillWPoison(struct stack* stk, int left, int right)
{
    DBG assert(stk != NULL);

    for (int i = left; i < right; i++)
    {
        stk->data[i] = GetPoison(stk->data[0]);
    }

    return NOERR;
}

elem_t StackPop(struct stack* stk)
{
    DBG assert(stk != NULL);

    if (int errors = StackErr(stk))
        DBG StackDump(stk, errors, __LINE__, __func__, __FILE__);

    stk->size--;
    elem_t value = stk->data[stk->size];
    stk->data[stk->size] = GetPoison(stk->data[0]);

    if (stk->size < SHRINKCOEFF * stk->size)
    {
        stk->capacity /= RESIZECOEFF;
        StackShrink(stk);
    }

    UpdateHash(stk);

    return value;
}

int StackPush(struct stack* stk, elem_t elem)
{
    DBG assert(stk != NULL);

    if (int errors = StackErr(stk))
        DBG StackDump(stk, errors, __LINE__, __func__, __FILE__);

    if (stk->size >= stk->capacity)
    {
        stk->capacity *= RESIZECOEFF;
        StackRealloc(stk);
        FillWPoison(stk, stk->size + 1, stk->capacity);
    }

    stk->data[stk->size++] = elem;

    UpdateHash(stk);

    return NOERR;
}

int StackRealloc(struct stack* stk)
{
    DBG assert(stk != NULL);

    char* prev = (char*) stk->dataguardl;

    char* buffer = (char*) realloc(stk->dataguardl, (stk->capacity) * sizeof(elem_t) + 2 * sizeof(unsigned long long));

    if (buffer == NULL)
        return MEMERR;

    DataInit

    if (prev != (char*) stk->dataguardl)
        free(prev);

    return NOERR;
}

int StackShrink(struct stack* stk)
{
    DBG assert(stk != NULL);

    char* buffer = (char*) realloc(stk->dataguardl, (stk->capacity) * sizeof(elem_t) + 2 * sizeof(unsigned long long));

    if (buffer == NULL)
        return MEMERR;

    DataInit

    return NOERR;
}

int StackErr(struct stack* stk)
{
    int errors = 0;
    unsigned int prevhash = stk->structhash;
    stk->structhash = 0;

    if (stk == NULL)
        return STKPTRERR;

    if (stk->data == NULL)
        errors |= DATAERR;
    if (stk->size < 0)
        errors |= SIZERR;
    if (stk->capacity < 0)
        errors |= CAPERR;
    if (stk->size > stk->capacity)
        errors |= SIZENCAPERR;
    if (stk->canaryleft != CANARY || stk->canaryright != CANARY || stk->dataguardl[0] != CANARY || stk->dataguardr[0] != CANARY)
        errors |= CANERR;
    if (MurMurHash(stk->data, stk->capacity * sizeof(elem_t), Seed) != stk->datahash)
        errors |= HASHERR;
    if (MurMurHash(stk, sizeof(*stk), Seed) != prevhash)
        errors |= HASHERR;

    return errors;
}

int StackDump(struct stack* stk, int errors, int line, const char* func, const char* file)
{
    FILE* logs = fopen("logs.txt", "a");

    if (stk == NULL)
    {
        fprintf(logs, "STACK POINTER IS NULL");
        return STKPTRERR;
    }

    fprintf(logs, "\n%s at ", func);
    fprintf(logs, "%s", file);
    fprintf(logs, "(%d)\n", line);
    fprintf(logs, "Stack[%p] ", &stk);
    if (errors == 0)
        fprintf(logs, "(OK)\n");
    else
        fprintf(logs, "(ERROR CODE = %d)\n", errors);
    fprintf(logs, "{\n\tsize = %lu\n", stk->size);
    fprintf(logs, "\tcapacity = %lu\n", stk->capacity);
    fprintf(logs, "\tdata[%p]\n\t{\n", stk->data);
    if (stk->data != NULL)
    {
        for (int i = 0; i < stk->capacity; i++)
        {
            if (stk->data[i] != GetPoison(stk->data[0]))
            {
                fprintf(logs, "\t\t*[%d] = ", i);
                print(logs, stk->data[i]);
            }
            else
                fprintf(logs, "\t\t[%d] = POISON\n", i);
        }
    }
    fprintf(logs, "\t}\n");
    fprintf(logs, "}\n");

    fclose(logs);

    return NOERR;
}

int StackDetor(struct stack* stk)
{
    DBG assert(stk != NULL);

    if (int errors = StackErr(stk))
        DBG StackDump(stk, errors, __LINE__, __func__, __FILE__);

    stk->canaryleft = 0;
    stk->canaryright = 0;
    stk->capacity = 0xAB0BA;
    free(stk->dataguardl);
    stk->data = NULL;
    stk->dataguardl = NULL;
    stk->dataguardr = NULL;
    stk->size = -1;
    stk->datahash = 0;
    stk->structhash = 0;

    return NOERR;
}

int DataFill(struct stack* stk, char* buffer)
{
    DBG assert(stk != NULL);

    stk->dataguardl = (unsigned long long*) buffer;
    stk->dataguardl[0] = CANARY;
    stk->data = (elem_t*) (buffer + sizeof(unsigned long long));
    stk->dataguardr = (unsigned long long*) (buffer + sizeof(unsigned long long) + stk->capacity * sizeof(elem_t));
    stk->dataguardr[0] = CANARY;

    return NOERR;
}

unsigned int MurMurHash(const void* data, int lenght, unsigned int seed)
{
    const int shift = 24;
    const unsigned int base = 0x5bd1e995;

    unsigned int hash = seed ^ lenght;

    const unsigned char* buffer = (const unsigned char*) data;

    while(lenght >= 4)
    {
        unsigned int curr = *(unsigned int*) buffer;

        curr *= base;
        curr ^= curr >> shift;
        curr *= base;

        hash *= base;
        hash ^= curr;

        buffer += 4;
        lenght -= 4;
    }

    switch (lenght)
    {
        case 3:
            hash ^= buffer[2] << 16;
        case 2:
            hash ^= buffer[1] << 8;
        case 1:
            hash ^= buffer[0];
            hash *= base;
    };

    hash ^= hash >> 13;
    hash *= base;
    hash ^= hash >> 15;

    return hash;
}

void UpdateHash(struct stack* stk)
{
    DBG assert(stk != NULL);

    stk->datahash = 0;
    stk->structhash = 0;

    stk->datahash = MurMurHash(stk->data, stk->capacity * sizeof(elem_t), Seed);
    stk->structhash = MurMurHash(stk, sizeof(*stk), Seed);
}

int print(FILE* fp, int x)
{
    return fprintf(fp, "%d\n", x);
}

int print(FILE* fp, double x)
{
    return fprintf(fp, "%lg\n", x);
}

int print(FILE* fp, char x)
{
    return fprintf(fp, "%c\n", x);
}

int print(FILE* fp, char* x)
{
    return fprintf(fp, "%p\n", x);
}

int print(FILE* fp, long x)
{
    return fprintf(fp, "%ld\n", x);
}

double GetPoison(double x)
{
    return 0xAB0BAB0BA48AB0BA;
}

int GetPoison(int x)
{
    return 0xDED32DED32DED32DED;
}

char GetPoison(char x)
{
    return '\0';
}

char* GetPoison(char* x)
{
    return NULL;
}

long GetPoison(long x)
{
    return 0xDED32DED;
}
