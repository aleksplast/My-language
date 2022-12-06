#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "Common.h"
#include "text-sort.h"

typedef struct poem source;

int AssemblerMain(struct asemblr* asemblr);

const char* GetComArg(int argc, char* argv[]);

int AsmCtor(struct asemblr* asemblr, const char* input);

int AsmDetor(struct asemblr* asemblr);

int CheckReg(const char* reg);

int GetArg(char* arg, struct asemblr* asemblr);

int GetLabel(struct asemblr* asemblr, int label);

int GetRamArg(char arg[], struct asemblr* asemblr);

int CheckLabel(struct asemblr* asemblr, int label);

int ListingPrint(FILE* out, void* arg, size_t size);

bool CheckForComment(char* line);

int PushLabel(struct asemblr* asemblr, char* cmd);

struct labels
{
    int* labelsarray;
    int labelsnum;
};

struct asminfo
{
    int sign;
    int version;
    int numofel;
};

struct asemblr
{
    FILE* listing;
    struct asminfo header;
    source text;
    char* code;
    struct labels labels;
    int ip;
};

enum AsmErrors
{
    REGERR = 1,
    LABELERR,
    ARGERR,
};

#endif //ASSEMBLER_H
