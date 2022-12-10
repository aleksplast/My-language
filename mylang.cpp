#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <strings.h>
#include <ctype.h>

#include "mylang.h"
#include "TXLib.h"

const int STRSIZE = 50;
const int NAMETABLESIZE = 30;

int LanguageMain(const char* input)
{
    setlocale(LC_ALL,"Rus");
    Source datasrc = InputHandler(input);

    FILE* asmprog = fopen("asmprog.txt", "w");

    Tree tree = {};
    char log[STRSIZE] = "pictures\\graphlog.htm";

    char** namestable = (char**) calloc(datasrc.size, sizeof(char*));

    char* datastr = TranslitIntoEnglish(datasrc.ptr, datasrc.size);

    Node** nodes = LexicAnalizer(datastr, &namestable);

    Node* node = GetGrammar(nodes);

    DataPrint(node);

    TreeCtor(&tree, NUM_TYPE, 0, OP_UNKNOWN, NULL, log);
    tree.anchor = node;
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);

    CreateAncestor(tree.anchor, NULL, &tree);

    CreateAsmProgramm(tree.anchor, asmprog);

    system("asm.exe asmprog.txt");
    system("cpu.exe");

    return NOERR;
}

int CreateAsmProgramm(Node* node, FILE* out)
{
    int labelcounter = 1;
    stack nametablestk = {};

    StackCtor(&nametablestk, 10);

    NamesTable table = CreateNameTable();
    StackPush(&nametablestk, table);
    FillNamesTable(node, &nametablestk);
    EnlargeRax(out, &nametablestk);

    fprintf(out, "call :main\n");
    fprintf(out, "hlt\n");

    NodeIntoAsmCode(node, &labelcounter, &nametablestk, out);

    fclose(out);

    return NOERR;
}

int NodeIntoAsmCode(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (node->optype == OP_STAT)
        AsmCodeStatement(node, labelcounter, nametablestk, out);

    else if (node->optype == OP_FUNC)
        AsmCodeFunc(node, labelcounter, nametablestk, out);

    else if (node->optype == OP_WHILE)
        AsmCodeWhile(node, labelcounter, nametablestk, out);

    else if (node->type == NUM_TYPE)
        AsmCodeNum(node, out);

    else if (node->optype == OP_IF)
        AsmCodeIf(node, labelcounter, nametablestk, out);

    else if (node->optype <= 4 && node->optype > 0)
        AsmCodeArithOper(node, labelcounter, nametablestk, out);

    else if (node->optype == OP_VAR)
        AsmCodeVar(node, labelcounter, nametablestk, out);

    else if (node->optype == OP_EQ)
        AsmCodeEq(node, labelcounter, nametablestk, out);

    else if (node->optype == OP_PARAM)
        AsmCodeParam(node, labelcounter, nametablestk, out);

    else if (node->optype == OP_RET)
        AsmCodeRet(node, labelcounter, nametablestk, out);

    else if (node->type == VAR_TYPE)
        AsmCodeVariable(node, nametablestk, out);

    else if (node->optype == OP_CALL)
        AsmCodeCall(node, labelcounter, nametablestk, out);

    else if (node->optype == OP_ELSE)
        AsmCodeElse(node, labelcounter, nametablestk, out);

    else if (node->optype == OP_OUT)
        AsmCodeOut(node, labelcounter, nametablestk, out);

    else if (node->optype == OP_IN)
        AsmCodeIn(node, labelcounter, nametablestk, out);

    return NOERR;
}

int AsmCodeStatement(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (node->leftchild)
        NodeIntoAsmCode(node->leftchild, labelcounter, nametablestk, out);

    if (node->rightchild)
        NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);

    return NOERR;
}

int AsmCodeArithOper(Node* node, int* labelcounter,stack* nametablestk, FILE* out)
{
    if (node->leftchild)
        NodeIntoAsmCode(node->leftchild, labelcounter, nametablestk, out);

    if (node->rightchild)
        NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);

    OpTypePrint(out, node->optype);
    fprintf(out, "\n");

    return NOERR;
}

int AsmCodeWhile(Node* node, int* labelcounter,stack* nametablestk, FILE* out)
{
    int whilelabel = *labelcounter;

    NamesTable table = CreateNameTable();
    StackPush(nametablestk, table);
    FillNamesTable(node->rightchild, nametablestk);

    EnlargeRax(out, nametablestk);

    fprintf(out, "%d:\n", whilelabel);
    (*labelcounter)++;

    NodeIntoAsmCode(node->leftchild, labelcounter, nametablestk, out);

    int skiplabel = *labelcounter;
    (*labelcounter)++;

    fprintf(out, "push 0\n");
    fprintf(out, "je :%d\n", skiplabel);

    NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);

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
    NamesTable table = CreateNameTable();
    StackPush(nametablestk, table);
    FillNamesTable(node, nametablestk);

    fprintf(out, "%s:\n", node->leftchild->varvalue);
    EnlargeRax(out, nametablestk);

    if (node->leftchild->leftchild)
        NodeIntoAsmCode(node->leftchild->leftchild, labelcounter, nametablestk, out);

    NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);

    StackPop(nametablestk);

    return NOERR;
}

int AsmCodeIf(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    NamesTable table = CreateNameTable();
    StackPush(nametablestk, table);

    NodeIntoAsmCode(node->leftchild, labelcounter, nametablestk, out);

    int skiplabel = *labelcounter;
    (*labelcounter)++;

    fprintf(out, "push 0\n");
    fprintf(out, "je :%d\n", skiplabel);

//    NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);
    if (node->rightchild->optype == OP_ELSE)
    {
        FillNamesTable(node->rightchild->leftchild, nametablestk);
        EnlargeRax(out, nametablestk);

        NodeIntoAsmCode(node->rightchild->leftchild, labelcounter, nametablestk, out);
        ReduceRax(out, nametablestk);
        StackPop(nametablestk);

        int elselabel = *labelcounter;
        (*labelcounter)++;

        fprintf(out, "jmp :%d\n", elselabel);
        fprintf(out, "%d:\n", skiplabel);

        NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);
        fprintf(out, "%d:\n", elselabel);
    }
    else
    {
        FillNamesTable(node->rightchild, nametablestk);
        EnlargeRax(out, nametablestk);

        NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);
        ReduceRax(out, nametablestk);
        fprintf(out, "%d:\n", skiplabel);

        StackPop(nametablestk);
    }

    return NOERR;
}

int AsmCodeElse(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    NamesTable table = CreateNameTable();
    StackPush(nametablestk, table);
    FillNamesTable(node->rightchild, nametablestk);
    EnlargeRax(out, nametablestk);

    if (node->rightchild)
        NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);

    ReduceRax(out, nametablestk);
    StackPop(nametablestk);

    return NOERR;
}

int AsmCodeVar(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (node->rightchild)
        NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);

    NodeIntoAsmCode(node->leftchild, labelcounter, nametablestk, out);

    return NOERR;
}

int AsmCodeEq(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);

    NodeIntoAsmCode(node->leftchild, labelcounter, nametablestk, out);

    return NOERR;
}

int AsmCodeParam(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    NodeIntoAsmCode(node->leftchild, labelcounter, nametablestk, out);

    if (node->rightchild)
        NodeIntoAsmCode(node->rightchild, labelcounter, nametablestk, out);

    return NOERR;
}

int AsmCodeRet(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (node->leftchild)
        NodeIntoAsmCode(node->leftchild, labelcounter, nametablestk, out);

    ReduceRax(out, nametablestk);
    fprintf(out, "ret\n");

    return NOERR;
}

int AsmCodeVariable(Node* node, stack* nametablestk, FILE* out)
{
    if (node->ancestor->optype == OP_VAR)
    {
        if (node->ancestor->rightchild)
        {
            PopFromNamesTable(out, nametablestk, node->varvalue);
        }
        else if (node->ancestor->ancestor->optype == OP_PARAM)
        {
            PopFromNamesTable(out, nametablestk, node->varvalue);
        }
    }
    else if (node->ancestor->optype == OP_PARAM)
        PushFromNamesTable(out, nametablestk, node->varvalue);
    else if (node->ancestor->optype == OP_EQ && node->ancestor->leftchild == node)
        PopFromNamesTable(out, nametablestk, node->varvalue);
    else if (node->ancestor->optype == OP_OUT)
        PushFromNamesTable(out, nametablestk, node->varvalue);
    else if (node->ancestor->optype == OP_IN)
        PopFromNamesTable(out, nametablestk, node->varvalue);
    else if (node->ancestor->optype == OP_EQ && node->ancestor->rightchild == node)
        PushFromNamesTable(out, nametablestk, node->varvalue);
    else if (node->ancestor->optype == OP_FUNC){;}
    else if (node->ancestor->optype == OP_CALL){;}
    else
        PushFromNamesTable(out, nametablestk, node->varvalue);

    return NOERR;
}

int AsmCodeCall(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    if (node->leftchild->leftchild)
        NodeIntoAsmCode(node->leftchild->leftchild, labelcounter, nametablestk, out);

    fprintf(out, "call :%s\n", node->leftchild->varvalue);

    return NOERR;
}

int AsmCodeOut(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    NodeIntoAsmCode(node->leftchild, labelcounter, nametablestk, out);

    fprintf(out, "out\n");

    return NOERR;
}

int AsmCodeIn(Node* node, int* labelcounter, stack* nametablestk, FILE* out)
{
    fprintf(out, "in\n");
    NodeIntoAsmCode(node->leftchild, labelcounter, nametablestk, out);

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

int FillNamesTable(Node* node, stack* namestablestk)
{
    if (node->leftchild)
    {
        if (node->leftchild->optype != OP_IF && node->leftchild->optype != OP_ELSE && node->leftchild->optype != OP_FUNC && node->leftchild->optype != OP_WHILE)
            FillNamesTable(node->leftchild, namestablestk);
    }

    if (node->rightchild)
    {
        if (node->rightchild->optype != OP_IF && node->rightchild->optype != OP_ELSE && node->rightchild->optype != OP_FUNC && node->rightchild->optype != OP_WHILE)
            FillNamesTable(node->rightchild, namestablestk);
    }

    if (node->optype == OP_VAR)
    {
        AddToNamesTable(namestablestk, node->leftchild->varvalue);
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
            case USELESS2:
                break;
            case USELESS1:
                break;
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
