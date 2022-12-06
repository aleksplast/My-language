#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <strings.h>
#include <ctype.h>

#include "mylang.h"
#include "TXLib.h"

const int STRSIZE = 50;
static int labelcounter = 1;

int LanguageMain(char* input)
{
    setlocale(LC_ALL,"Rus");
    Source datasrc = InputHandler(input);

    FILE* asmprog = fopen("asmprog.txt", "w");

    Tree tree = {};
    char log[STRSIZE] = "pictures\\graphlog.htm";

    char* datastr = TranslitIntoEnglish(datasrc.ptr, datasrc.size);

    Node** nodes = LexicAnalizer(datastr);

    Node* node = GetGrammar(nodes);

    TreeCtor(&tree, NUM_TYPE, 0, OP_UNKNOWN, NULL, log);
    tree.anchor = node;
    TreeGraphDump(&tree, 0, __LINE__, __func__, __FILE__);

    CreateAncestor(tree.anchor, NULL, &tree);

    CreateAsmProgramm(tree.anchor, asmprog);

    return NOERR;
}

int CreateAsmProgramm(Node* node, FILE* out)
{
    NodeIntoAsmCode(node, out);

    fclose(out);

    return NOERR;
}

int NodeIntoAsmCode(Node* node, FILE* out)
{
    if (node->optype == OP_FUNC)
    {
        fprintf(out, ":%d\n", labelcounter);
    }

    if (node->leftchild)
        NodeIntoAsmCode(node->leftchild, out);

    if (node->type == NUM_TYPE)
        fprintf(out, "push %lg\n", node->val);

    if (node->rightchild)
        NodeIntoAsmCode(node->rightchild, out);

    if (node->optype <= 4 && node->optype > 0)
    {
        OpTypePrint(out, node->optype);
        fprintf(out, "\n");
    }

    return NOERR;
}

Source InputHandler(char* input)
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
