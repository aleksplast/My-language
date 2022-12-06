#ifndef COMMON_H
#define COMMON_H

const int version = 1;
typedef double elem_t;

enum ComArgs
{
    ARG_MEM     = 1 << 7,
    ARG_REG     = 1 << 6,
    ARG_IMMED   = 1 << 5,
};

#define DEF_CMD(name, num, arg, code)                                   \
    CMD_##name = num,
enum Commands
{
#include "..\Assembler\cmd.h"
    NOCMD = 31
};
#undef DEF_CMD

#endif //COMMON_H
