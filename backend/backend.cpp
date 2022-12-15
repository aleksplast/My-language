#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <strings.h>
#include <ctype.h>

#include "..\backend\backend.h"
#include "..\functions\TXLib.h"

const int NAMETABLESIZE = 30;
const OperType FirstIf = OP_IS_EE;

#define AsmCode(node) NodeIntoAsmCode(node, labelcounter, nametablestk, out);
#define PrepNewTable(node)      NamesTable table = CreateNameTable();   \
                                StackPush(nametablestk, table);         \
                                FillNamesTable(node, nametablestk);
#define AsmProgFunc(name) AsmCode##name(node, labelcounter, nametablestk, out)
#define PopFTable PopFromNamesTable(out, nametablestk, node->varvalue)
#define PushFTable PushFromNamesTable(out, nametablestk, node->varvalue)
#define IS_OP(node, oper) (node->optype == oper)

int CreateAsmProgramm(Node* node, FILE* out)
{
    int labelcounter = 1;
    stack nametablestk = {};
    fprintf(out,    "push 2\n"
                    "pop rax\n");

    StackCtor(&nametablestk, 10);

    PrepareProg(node, out, &nametablestk);
    NodeIntoAsmCode(node, &labelcounter, &nametablestk, out);
    AddCoreFunctions(out);

    fclose(out);

    return NOERR;
}

int PrepareProg(Node* node, FILE* out, stack* nametablestk)
{
    PrepNewTable(node)
    EnlargeRax(out, nametablestk);

    fprintf(out, "call :main\n");
    fprintf(out, "hlt\n");

    return NOERR;
}

int AddCoreFunctions(FILE* out)
{
    fprintf(out,    "\nzhmurik:\n"
                    "out\n"
                    "ret\n");
    fprintf(out,    "\ntopdek:\n"
                    "in\n"
                    "ret\n");

    fprintf(out,    "\nsrezat\':\n"
                    "sqrt\n"
                    "ret\n");

    return NOERR;
}

int NodeIntoAsmCode(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    switch (node->type)
    {
        case NUM_TYPE:
            AsmCodeNum(node, out);
            break;
        case VAR_TYPE:
            AsmCodeVariable(node, nametablestk, out);
            break;
        case OP_TYPE:
            switch (node->optype)
            {
                case OP_STAT:
                    AsmProgFunc(Statement);
                    break;
                case OP_FUNC:
                    AsmProgFunc(Func);
                    break;
                case OP_WHILE:
                    AsmProgFunc(While);
                    break;
                case OP_IF:
                    AsmProgFunc(If);
                    break;
                case OP_VAR:
                    AsmProgFunc(Var);
                    break;
                case OP_EQ:
                    AsmProgFunc(Eq);
                    break;
                case OP_PARAM:
                    AsmProgFunc(Param);
                    break;
                case OP_RET:
                    AsmProgFunc(Ret);
                    break;
                case OP_CALL:
                    AsmProgFunc(Call);
                    break;
                case OP_ELSE:
                    AsmProgFunc(Else);
                    break;
                case OP_ADD:
                    AsmProgFunc(ArithOper);
                    break;
                case OP_SUB:
                    AsmProgFunc(ArithOper);
                    break;
                case OP_MUL:
                    AsmProgFunc(ArithOper);
                    break;
                case OP_DIV:
                    AsmProgFunc(ArithOper);
                    break;
                default:
                    break;
            }
        default:
            break;
    }

    return NOERR;
}

int AsmCodeStatement(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (node->leftchild)
        AsmCode(node->leftchild);

    if (node->rightchild)
        AsmCode(node->rightchild);

    return NOERR;
}

int AsmCodeArithOper(Node* node, int* labelcounter,stack* nametablestk, FILE* out)
{
    AsmCode(node->leftchild);
    AsmCode(node->rightchild);
    OpTypePrint(out, node->optype);
    fprintf(out, "\n");

    return NOERR;
}

int AsmCodeWhile(Node* node, int* labelcounter,stack* nametablestk, FILE* out)
{
    int whilelabel = *labelcounter;

    PrepNewTable(node->rightchild)

    EnlargeRax(out, nametablestk);

    fprintf(out, "%d:\n", whilelabel);
    (*labelcounter)++;

    int skiplabel = *labelcounter;
    (*labelcounter)++;

    if (node->leftchild->optype >= OP_IS_EE)
    {
        AsmCode(node->rightchild);
        AsmCode(node->leftchild);

        JumpPrint(out, node->leftchild->optype, skiplabel);
    }
    else
    {
        AsmCode(node->leftchild);

        fprintf(out, "push 0\n");
        fprintf(out, "je :%d\n", skiplabel);
    }

    AsmCode(node->rightchild);

    fprintf(out, "jmp :%d\n", whilelabel);
    fprintf(out, "%d:\n", skiplabel);

    ReduceRax(out, nametablestk);
    StackPop(nametablestk);

    return NOERR;
}

int AsmCodeNum(Node* node, FILE* out)
{
    fprintf(out, "push %lg\n", node->val);

    return NOERR;
}

int AsmCodeFunc(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (node->rightchild == NULL)
        return NOERR;

    PrepNewTable(node)

    fprintf(out, "%s:\n", node->leftchild->varvalue);
    EnlargeRax(out, nametablestk);

    if (node->leftchild->leftchild)
        AsmCode(node->leftchild->leftchild);

    AsmCode(node->rightchild);

    StackPop(nametablestk);

    return NOERR;
}

int AsmCodeIf(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    NamesTable table = CreateNameTable();
    StackPush(nametablestk, table);

    int skiplabel = *labelcounter;
        (*labelcounter)++;

    if (node->leftchild->optype >= OP_IS_EE)
    {
        AsmCode(node->leftchild->rightchild);
        AsmCode(node->leftchild->leftchild);

        JumpPrint(out, node->leftchild->optype, skiplabel);
    }
    else
    {
        AsmCode(node->leftchild);

        fprintf(out, "push 0\n");
        fprintf(out, "je :%d\n", skiplabel);
    }

//    NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);
    if (node->rightchild->optype == OP_ELSE)
    {
        FillNamesTable(node->rightchild->leftchild, nametablestk);
        EnlargeRax(out, nametablestk);

        AsmCode(node->rightchild->leftchild);
        ReduceRax(out, nametablestk);
        StackPop(nametablestk);

        int elselabel = *labelcounter;
        (*labelcounter)++;

        fprintf(out, "jmp :%d\n", elselabel);
        fprintf(out, "%d:\n", skiplabel);

        AsmCode(node->rightchild);
        fprintf(out, "%d:\n", elselabel);
    }
    else
    {
        FillNamesTable(node->rightchild, nametablestk);
        EnlargeRax(out, nametablestk);

        AsmCode(node->rightchild);
        ReduceRax(out, nametablestk);
        fprintf(out, "%d:\n", skiplabel);

        StackPop(nametablestk);
    }

    return NOERR;
}

int AsmCodeElse(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    PrepNewTable(node->rightchild);
    EnlargeRax(out, nametablestk);

    if (node->rightchild)
        AsmCode(node->rightchild);

    ReduceRax(out, nametablestk);
    StackPop(nametablestk);

    return NOERR;
}

int AsmCodeVar(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (node->rightchild)
        AsmCode(node->rightchild);

    AsmCode(node->leftchild);

    return NOERR;
}

int AsmCodeEq(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    AsmCode(node->rightchild);

    AsmCode(node->leftchild);

    return NOERR;
}

int AsmCodeParam(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (IS_OP(node->leftchild, OP_VAR))
    {
        if (node->rightchild)
            AsmCode(node->rightchild);

        AsmCode(node->leftchild);
    }
    else
    {
        AsmCode(node->leftchild);

        if (node->rightchild)
            AsmCode(node->rightchild);
    }

    return NOERR;
}

int AsmCodeRet(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (node->leftchild)
        AsmCode(node->leftchild);

    ReduceRax(out, nametablestk);
    fprintf(out, "ret\n");

    return NOERR;
}

int AsmCodeVariable(Node* node, stack* nametablestk, FILE* out)
{
    if (IS_OP(node->ancestor, OP_VAR))
    {
        if (node->ancestor->rightchild)
        {
            PopFTable;
        }
        else if (IS_OP(node->ancestor->ancestor, OP_PARAM))
        {
            PopFTable;
        }
    }
    else if (IS_OP(node->ancestor, OP_PARAM) && stricmp(node->ancestor->ancestor->varvalue, "topdek") == 0)
        {;}
    else if (IS_OP(node->ancestor, OP_PARAM))
        PushFTable;
    else if (IS_OP(node->ancestor, OP_EQ) && node->ancestor->leftchild == node)
        PopFTable;
    else if (IS_OP(node->ancestor, OP_EQ) && node->ancestor->rightchild == node)
        PushFTable;
    else if (IS_OP(node->ancestor, OP_FUNC)){;}
    else if (IS_OP(node->ancestor, OP_CALL)){;}
    else
        PushFTable;

    return NOERR;
}

int AsmCodeCall(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (node->leftchild->leftchild)
        AsmCode(node->leftchild->leftchild);

    fprintf(out, "call :%s\n", node->leftchild->varvalue);

    if (stricmp(node->leftchild->varvalue, "topdek") == 0)
        PopFromNamesTable(out, nametablestk, node->leftchild->leftchild->leftchild->varvalue);

    return NOERR;
}

int AsmCodeOut(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    AsmCode(node->leftchild);

    fprintf(out, "out\n");

    return NOERR;
}

int AsmCodeIn(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    fprintf(out, "in\n");
    AsmCode(node->leftchild);

    return NOERR;
}

int EnlargeRax(FILE* out, stack* nametablestk)
{
    NamesTable table = StackPop(nametablestk);

    fprintf(out, "\npush rax            #увеличить сдвиг\n");
    fprintf(out, "push %d\n", table.size);
    fprintf(out, "add\n");
    fprintf(out, "pop rax\n\n");

    StackPush(nametablestk, table);

    return NOERR;
}

int ReduceRax(FILE* out, stack* nametablestk)
{
    NamesTable table = StackPop(nametablestk);

    fprintf(out, "\npush rax          #уменьшить сдвиг\n");
    fprintf(out, "push %d\n", table.size);
    fprintf(out, "sub\n");
    fprintf(out, "pop rax\n\n");

    StackPush(nametablestk, table);

    return NOERR;
}

NamesTable CreateNameTable()
{
    NamesTable table = {};

    table.names = (Name*) calloc(NAMETABLESIZE, sizeof(Name));
    table.size = 0;

    return table;
}

int AddToNamesTable(stack* nametablestk, char* name)
{
    NamesTable table = StackPop(nametablestk);

    table.names[table.size].name = name;
    table.names[table.size].index = table.size;
    table.size++;

    StackPush(nametablestk, table);

    return NOERR;
}

int PushFromNamesTable(FILE* out, stack* nametablestk, char* name)
{
    int pos = 0;
    for (int i = (int)nametablestk->size - 1; i >= 0; i--)
    {
        pos += nametablestk->data[i].size;
        for (int j = (int)nametablestk->data[i].size - 1; j >= 0; j--)
        {
            if (stricmp(nametablestk->data[i].names[j].name, name) == 0)
            {
                pos = nametablestk->data[i].names[j].index - pos;
                fprintf(out, "push [%d+rax]\n", pos);
                return NOERR;
            }
        }
    }

    return NOERR;
}

int PopFromNamesTable(FILE* out, stack* nametablestk, char* name)
{
    int pos = 0;

    for (int i = (int)nametablestk->size - 1; i >= 0; i--)
    {
        pos += nametablestk->data[i].size;

        for (int j = (int)nametablestk->data[i].size - 1; j >= 0; j--)
        {
            if (stricmp(nametablestk->data[i].names[j].name, name) == 0)
            {
                pos = nametablestk->data[i].names[j].index - pos;
                fprintf(out, "pop [%d+rax]\n", pos);
                return NOERR;
            }
        }
    }

    return NOERR;
}

int PopBackwardsFromNamesTable(FILE* out, stack* nametablestk, char* name)
{
    int pos = 0;

    for (int i = (int)nametablestk->size - 1; i >= 0; i--)
    {
        pos += nametablestk->data[i].size;

        for (int j = (int)nametablestk->data[i].size - 1; j >= 0; j--)
        {
            if (stricmp(nametablestk->data[i].names[j].name, name) == 0)
            {
                pos = nametablestk->data[i].names[j].index - pos;
                fprintf(out, "pop [%d+rax]\n", -1 * nametablestk->data[i].size - pos - 1);
                return NOERR;
            }
        }
    }

    return NOERR;
}

int FillNamesTable(Node* node, stack* namestablestk)
{
    if (node->leftchild)
    {
        if (!IS_OP(node->leftchild, OP_IF) && !IS_OP(node->leftchild, OP_ELSE) && !IS_OP(node->leftchild, OP_FUNC) && !IS_OP(node->leftchild, OP_WHILE))
            FillNamesTable(node->leftchild, namestablestk);
    }

    if (node->rightchild)
    {
        if (!IS_OP(node->rightchild, OP_IF) && !IS_OP(node->rightchild, OP_ELSE) && !IS_OP(node->rightchild, OP_FUNC) && !IS_OP(node->rightchild, OP_WHILE))
            FillNamesTable(node->rightchild, namestablestk);
    }

    if (node->optype == OP_VAR)
        AddToNamesTable(namestablestk, node->leftchild->varvalue);

    return NOERR;
}

int JumpPrint(FILE* out, OperType optype, int label)
{
    switch (optype)
    {
        case OP_IS_EE:
            fprintf(out, "jne :%d\n", label);
            break;
        case OP_IS_GE:
            fprintf(out, "jb :%d\n", label);
            break;
        case OP_IS_BE:
            fprintf(out, "ja :%d\n", label);
            break;
        case OP_IS_GT:
            fprintf(out, "jbe :%d\n", label);
            break;
        case OP_IS_BT:
            fprintf(out, "jae :%d\n", label);
            break;
        case OP_IS_NE:
            fprintf(out, "je :%d\n", label);
            break;
        default:
            break;
    }

    return NOERR;
}

Source InputHandler(const char* input)
{
    const char* mode = "r";

    Source src = {};

    TextReader(input, &src, mode);

    return src;
}

char* TranslitIntoEnglish(char* str, long size)
{
    char* newstr = (char*) calloc(4 * size, sizeof(char));
    char* result = newstr;

    while (*str != '\0')
    {
        switch (*str)
        {
            case ARUS:
                *newstr = 'a';
                newstr++;
                break;
            case BRUS:
                *newstr = 'b';
                newstr++;
                break;
            case VRUS:
                *newstr = 'v';
                newstr++;
                break;
            case GRUS:
                *newstr = 'g';
                newstr++;
                break;
            case DRUS:
                *newstr = 'd';
                newstr++;
                break;
            case ERUS:
                *newstr = 'e';
                newstr++;
                break;
            case ZHRUS:
                *newstr = 'z';
                newstr++;
                *newstr = 'h';
                newstr++;
                break;
            case ZRUS:
                *newstr = 'z';
                newstr++;
                break;
            case IRUS:
                *newstr = 'i';
                newstr++;
                break;
            case YIRUS:
                *newstr = 'i';
                newstr++;
                break;
            case KRUS:
                *newstr = 'k';
                newstr++;
                break;
            case LRUS:
                *newstr = 'l';
                newstr++;
                break;
            case MRUS:
                *newstr = 'm';
                newstr++;
                break;
            case NRUS:
                *newstr = 'n';
                newstr++;
                break;
            case ORUS:
                *newstr = 'o';
                newstr++;
                break;
            case PRUS:
                *newstr = 'p';
                newstr++;
                break;
            case RRUS:
                *newstr = 'r';
                newstr++;
                break;
            case SRUS:
                *newstr = 's';
                newstr++;
                break;
            case TRUS:
                *newstr = 't';
                newstr++;
                break;
            case URUS:
                *newstr = 'u';
                newstr++;
                break;
            case FRUS:
                *newstr = 'f';
                newstr++;
                break;
            case HRUS:
                *newstr = 'k';
                newstr++;
                *newstr = 'h';
                newstr++;
                break;
            case TSRUS:
                *newstr = 't';
                newstr++;
                *newstr = 's';
                newstr++;
                break;
            case CHRUS:
                *newstr = 'c';
                newstr++;
                *newstr = 'h';
                newstr++;
                break;
            case SHRUS:
                *newstr = 's';
                newstr++;
                *newstr = 'h';
                newstr++;
                break;
            case SHCHRUS:
                *newstr = 's';
                newstr++;
                *newstr = 'h';
                newstr++;
                *newstr = 'c';
                newstr++;
                *newstr = 'h';
                newstr++;
                break;
            case IERUS:
                *newstr = 'i';
                newstr++;
                *newstr = 'e';
                newstr++;
                break;
            case YRUS:
                *newstr = 'y';
                newstr++;
                break;
            case QRUS:
                *newstr = '\'';
                newstr++;
                break;
            case EERUS:
                *newstr = 'e';
                newstr++;
                break;
            case IURUS:
                *newstr = 'i';
                newstr++;
                *newstr = 'u';
                newstr++;
                break;
            case IARUS:
                *newstr = 'i';
                newstr++;
                *newstr = 'a';
                newstr++;
                break;
            case ABRUS:
                *newstr = 'a';
                newstr++;
                break;
            case BBRUS:
                *newstr = 'b';
                newstr++;
                break;
            case VBRUS:
                *newstr = 'v';
                newstr++;
                break;
            case GBRUS:
                *newstr = 'g';
                newstr++;
                break;
            case DBRUS:
                *newstr = 'd';
                newstr++;
                break;
            case EBRUS:
                *newstr = 'e';
                newstr++;
                break;
            case ZHBRUS:
                *newstr = 'z';
                newstr++;
                *newstr = 'h';
                newstr++;
                break;
            case ZBRUS:
                *newstr = 'z';
                newstr++;
                break;
            case IBRUS:
                *newstr = 'i';
                newstr++;
                break;
            case YIBRUS:
                *newstr = 'i';
                newstr++;
                break;
            case KBRUS:
                *newstr = 'k';
                newstr++;
                break;
            case LBRUS:
                *newstr = 'l';
                newstr++;
                break;
            case MBRUS:
                *newstr = 'm';
                newstr++;
                break;
            case NBRUS:
                *newstr = 'n';
                newstr++;
                break;
            case OBRUS:
                *newstr = 'o';
                newstr++;
                break;
            case PBRUS:
                *newstr = 'p';
                newstr++;
                break;
            case RBRUS:
                *newstr = 'r';
                newstr++;
                break;
            case SBRUS:
                *newstr = 's';
                newstr++;
                break;
            case TBRUS:
                *newstr = 't';
                newstr++;
                break;
            case UBRUS:
                *newstr = 'u';
                newstr++;
                break;
            case FBRUS:
                *newstr = 'f';
                newstr++;
                break;
            case HBRUS:
                *newstr = 'k';
                newstr++;
                *newstr = 'h';
                newstr++;
                break;
            case TSBRUS:
                *newstr = 't';
                newstr++;
                *newstr = 's';
                newstr++;
                break;
            case CHBRUS:
                *newstr = 'c';
                newstr++;
                *newstr = 'h';
                newstr++;
                break;
            case SHBRUS:
                *newstr = 's';
                newstr++;
                *newstr = 'h';
                newstr++;
                break;
            case SHCHBRUS:
                *newstr = 's';
                newstr++;
                *newstr = 'h';
                newstr++;
                *newstr = 'c';
                newstr++;
                *newstr = 'h';
                newstr++;
                break;
            case IEBRUS:
                *newstr = 'i';
                newstr++;
                *newstr = 'e';
                newstr++;
                break;
            case YBRUS:
                *newstr = 'y';
                newstr++;
                break;
            case QBRUS:
                *newstr = '\'';
                newstr++;
                break;
            case EEBRUS:
                *newstr = 'e';
                newstr++;
                break;
            case IUBRUS:
                *newstr = 'i';
                newstr++;
                *newstr = 'u';
                newstr++;
                break;
            case IABRUS:
                *newstr = 'i';
                newstr++;
                *newstr = 'a';
                newstr++;
                break;
            default:
                *newstr = *str;
                newstr++;
                break;
        }

        str++;
    }
    *newstr = '\0';

    return result;
}
