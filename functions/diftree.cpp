#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

#include "diftree.h"
#include "common.h"
#include "TXLib.h"
#include "text-sort.h"

typedef poem Source;

const char* Poison = NULL;
const double EPSILON = 1e-10;

#define TREECHECK   if (int errors = TreeVerr(tree))                                         \
                        DBG TreeGraphDump(tree, errors, __LINE__, __func__, __FILE__);

static int Piccounter = 1;

#define CreateNum(val) CreateNode(NUM_TYPE, val, OP_UNKNOWN, NULL, NULL, currnode, NULL, NULL, 0)
#define CreateOpType(optype)  CreateNode(OP_TYPE, 0, optype, NULL, NULL, currnode, NULL, NULL, 0)
#define CreateVar(curr) CreateNode(VAR_TYPE, 0, OP_UNKNOWN, curr, NULL, currnode, NULL, NULL, 0)

int TreeCtor(Tree* tree, NodeType type, double val, OperType optype, char* varvalue, char* graphfile)
{
    DBG assert(tree != NULL);

    if ((tree->anchor = (Node*) calloc(1, sizeof(Node))) == NULL)
        return MEMERR;

    tree->anchor->type = type;
    tree->anchor->val = val;
    tree->anchor->optype = optype;
    tree->anchor->varvalue = varvalue;

    tree->anchor->ancestor = NULL;
    tree->anchor->rightchild = NULL;
    tree->anchor->leftchild = NULL;
    tree->anchor->tree = tree;
    tree->anchor->line = 0;

    tree->size = 1;

    tree->graphlog = graphfile;

    FILE* fp = fopen(tree->graphlog, "w");
    fclose(fp);

    TREECHECK

    return NOERR;
}

int TreeVerr(Tree* tree)
{
    DBG assert(tree != NULL);

    int errors = 0;

    if (tree->size < 0)
        errors += SIZEERR;

    return errors;
}

int NodeVerr (Node* node, int* errors)
{
    DBG assert(node != NULL);

    if (node->leftchild)
        *errors += NodeVerr(node->leftchild, errors);
    if (node->rightchild)
        *errors += NodeVerr(node->rightchild, errors);

    return 0;
}

int TreeGraphDump (Tree* tree, int errors, int line, const char* func, const char* file)
{
    DBG assert(tree != NULL);

    if (errors > 0){;}

    char picture[80] = "picture";
    sprintf(picture, "pictures\\%d.dot", Piccounter);
    char picturesrc[80] = "picture";
    sprintf(picturesrc, "pictures\\%d.png", Piccounter);

    FILE* pic = fopen(picture, "w");

    fprintf(pic, "strict graph {\n");
    fprintf(pic, "\trankdir = TB\n");

    int counter = 1;
    NodeDump(tree->anchor, &counter, pic);

    fprintf(pic, "}");

    fclose(pic);
    Piccounter += 1;

    char src[200] = "";
    sprintf(src, "dot -Tpng %s -o %s", picture, picturesrc);

    system(src);

    sprintf(picturesrc, "%d.png", Piccounter - 1);

    FILE* graphlog = fopen(tree->graphlog, "a");

    fprintf(graphlog, "<pre>\n");

    fprintf(graphlog, "<h2>TreeDump on %d line in %s in %s</h2>\n", line, func, file);
    fprintf(graphlog, "<img src = \"%s\">\n", picturesrc);

    fprintf(graphlog, "<hr>\n");

    fclose(graphlog);

    return NOERR;
}

int NodeDump(Node* node, int* counter, FILE* pic)
{
    int left = 0;
    int right = 0;
    if (node->leftchild)
        left = NodeDump(node->leftchild, counter, pic);
    if (node->rightchild)
        right = NodeDump(node->rightchild, counter, pic);

    fprintf(pic, "\t\"node%d\" [shape = \"record\", style = \"filled\", fillcolor = \"green\", label = \"{", *counter);
    TypePrint(pic, node->type);
    fprintf(pic, "|");
    if (node->optype)
    {
        OpTypePrint(pic, node->optype);
    }
    else if (compare(node->val, 0) != 0)
        fprintf(pic, "%lg", node->val);
    else if (node->varvalue)
        fprintf(pic, "%s", node->varvalue);
    else
        fprintf(pic, "%d", 0);
    fprintf(pic, "}\"]\n");
    int curr = *counter;
    if (node->leftchild)
        fprintf(pic, "\t\"node%d\" -- \"node%d\"\n", curr, left);
    if (node->rightchild)
        fprintf(pic, "\t\"node%d\" -- \"node%d\"\n", curr, right);

    (*counter)++;

    return *counter - 1;
}

Node* AddRightChild(Tree* tree, Node* ancestor)
{
    DBG assert(ancestor != NULL);

    if (ancestor->rightchild)
        return ancestor;

    if ((ancestor->rightchild = (Node*) calloc(1, sizeof(Node))) == NULL)
        return ancestor;

    tree->size++;

    ancestor->rightchild->type = UNKNOWN_TYPE;
    ancestor->rightchild->val = 0;
    ancestor->rightchild->optype = OP_UNKNOWN;
    ancestor->rightchild->varvalue = NULL;
    ancestor->rightchild->line = 0;

    ancestor->rightchild->leftchild = NULL;
    ancestor->rightchild->rightchild = NULL;
    ancestor->rightchild->ancestor = ancestor;
    ancestor->rightchild->tree = ancestor->tree;

    TREECHECK

    return ancestor->rightchild;
}

Node* AddLeftChild(Tree* tree, Node* ancestor)
{
    DBG assert(ancestor != NULL);

    if (ancestor->leftchild)
        return ancestor;

    if ((ancestor->leftchild = (Node*) calloc(1, sizeof(Node))) == NULL)
        return ancestor;

    tree->size++;

    ancestor->leftchild->type = UNKNOWN_TYPE;
    ancestor->leftchild->val = 0;
    ancestor->leftchild->optype = OP_UNKNOWN;
    ancestor->leftchild->varvalue = NULL;
    ancestor->leftchild->line = 0;

    ancestor->leftchild->leftchild = NULL;
    ancestor->leftchild->rightchild = NULL;
    ancestor->leftchild->ancestor = ancestor;
    ancestor->leftchild->tree = ancestor->tree;

    TREECHECK

    return ancestor->leftchild;
}

int NodeDetor(Node* node)
{
    DBG assert (node != NULL);

    if (node->leftchild)
        NodeDetor(node->leftchild);

    if (node->rightchild)
        NodeDetor(node->rightchild);

    node->leftchild = NULL;
    node->rightchild = NULL;
    node->ancestor = NULL;
    node->tree->size--;

    free(node);

    return NOERR;
}

Node* TreeDepthSearch(Node* node, char* searchvar)
{
    assert(node != NULL);

    Node* searchleft = NULL;
    Node* searchright = NULL;

    if (node->leftchild)
        searchleft = TreeDepthSearch(node->leftchild, searchvar);
    if (node->rightchild)
        searchright = TreeDepthSearch(node->rightchild, searchvar);

    if (stricmp(node->varvalue, searchvar) == 0)
        return node;
    else if (searchright)
        return searchright;
    else if (searchleft)
        return searchleft;
    else
        return NULL;
}

int TreeDetor(Tree* tree)
{
    DBG assert (tree != NULL);

//    TREECHECK

    if (tree->anchor)
        NodeDetor(tree->anchor);

    tree->size = -1;
    tree->graphlog = NULL;
    tree = NULL;

    return NOERR;
}

int TypePrint(FILE* fp, NodeType type)
{
    if (type == UNKNOWN_TYPE)
        fprintf(fp, "%s", "UNKNOWN");
    if (type == VAR_TYPE)
        fprintf(fp, "%s", "VAR");
    if (type == OP_TYPE)
        fprintf(fp, "%s", "OP");
    if (type == NUM_TYPE)
        fprintf(fp, "%s", "NUM");

    return NOERR;
}

int OpTypePrint(FILE* fp, OperType opertype)
{
    if (opertype == OP_UNKNOWN)
        fprintf(fp, "%s", "UNKNOWN");
    else if (opertype == OP_ADD)
        fprintf(fp, "%s", "ADD");
    else if (opertype == OP_MUL)
        fprintf(fp, "%s", "MUL");
    else if (opertype == OP_SUB)
        fprintf(fp, "%s", "SUB");
    else if (opertype == OP_DIV)
        fprintf(fp, "%s", "DIV");
    else if (opertype == OP_POWER)
        fprintf(fp, "%s", "^");
    else if (opertype == OP_OPBRC)
        fprintf(fp, "%s", "OPBRC");
    else if (opertype == OP_CLBRC)
        fprintf(fp, "%s", "CLBRC");
    else if (opertype == OP_OPBLOCK)
        fprintf(fp, "%s", "OPBLOCK");
    else if (opertype == OP_CLBLOCK)
        fprintf(fp, "%s", "CLBLOCK");
    else if (opertype == OP_PARAM)
        fprintf(fp, "%s", "PARAM");
    else if (opertype == OP_VAR)
        fprintf(fp, "%s", "VAR");
    else if (opertype == OP_FUNC)
        fprintf(fp, "%s", "FUNC");
    else if (opertype == OP_STAT)
        fprintf(fp, "%s", "ST");
    else if (opertype == OP_IF)
        fprintf(fp, "%s", "IF");
    else if (opertype == OP_ELSE)
        fprintf(fp, "%s", "ELSE");
    else if (opertype == OP_WHILE)
        fprintf(fp, "%s", "WHILE");
    else if (opertype == OP_RET)
        fprintf(fp, "%s", "RET");
    else if (opertype == OP_EQ)
        fprintf(fp, "%s", "EQ");
    else if (opertype == OP_CALL)
        fprintf(fp, "%s", "CALL");
    else if (opertype == OP_IS_EE)
        fprintf(fp, "%s", "IS_EE");
    else if (opertype == OP_IS_GE)
        fprintf(fp, "%s", "IS_GE");
    else if (opertype == OP_IS_BE)
        fprintf(fp, "%s", "IS_BE");
    else if (opertype == OP_IS_GT)
        fprintf(fp, "%s", "IS_GT");
    else if (opertype == OP_IS_BT)
        fprintf(fp, "%s", "IS_BT");
    else if (opertype == OP_IS_NE)
        fprintf(fp, "%s", "IS_NE");

    return NOERR;
}

OperType IsOper(char* str)
{
    if (stricmp(str, "raskrutit\'") == 0)
        return OP_MUL;
    else if (stricmp(str, "baff") == 0)
        return OP_ADD;
    else if (stricmp(str, "klif") == 0)
        return OP_DIV;
    else if (stricmp(str, "damag") == 0)
        return OP_SUB;
    else if (stricmp(str, "aptaverny") == 0)
        return OP_POWER;
    else if (stricmp(str, "zdarova") == 0)
        return OP_OPBLOCK;
    else if (stricmp(str, "poka") == 0)
        return OP_CLBLOCK;
    else if (stricmp(str, "podrubai") == 0)
        return OP_OPBRC;
    else if (stricmp(str, "ofai") == 0)
        return OP_CLBRC;
    else if (stricmp(str, "vlad") == 0)
        return OP_OR;
    else if (stricmp(str, "stas") == 0)
        return OP_AND;
    else if (stricmp(str, "vin") == 0)
        return OP_IF;
    else if (stricmp(str, "luz") == 0)
        return OP_ELSE;
    else if (stricmp(str, "katka") == 0)
        return OP_FUNC;
    else if (stricmp(str, "chat") == 0)
        return OP_SEP;
    else if (stricmp(str, "vylet") == 0)
        return OP_RET;
    else if (stricmp(str, "regat\'") == 0)
        return OP_WHILE;
    else if (stricmp(str, "karta") == 0)
        return OP_VAR;
    else if (stricmp(str, "staty") == 0)
        return OP_EQ;
    else if (stricmp(str, "iiiii") == 0)
        return OP_COMMA;
    else if (stricmp(str, "top1") == 0)
        return OP_IS_EE;
    else if (stricmp(str, "top2") == 0)
        return OP_IS_GE;
    else if (stricmp(str, "top3") == 0)
        return OP_IS_BE;
    else if (stricmp(str, "top4") == 0)
        return OP_IS_GT;
    else if (stricmp(str, "top5") == 0)
        return OP_IS_BT;
    else if (stricmp(str, "top6") == 0)
        return OP_IS_NE;

    return OP_UNKNOWN;
}

OperType IsStdOper(char* str)
{
    if (stricmp(str, "MUL") == 0)
        return OP_MUL;
    else if (stricmp(str, "ADD") == 0)
        return OP_ADD;
    else if (stricmp(str, "DIV") == 0)
        return OP_DIV;
    else if (stricmp(str, "SUB") == 0)
        return OP_SUB;
    else if (stricmp(str, "POW") == 0)
        return OP_POWER;
    else if (stricmp(str, "IF") == 0)
        return OP_IF;
    else if (stricmp(str, "ELSE") == 0)
        return OP_ELSE;
    else if (stricmp(str, "FUNC") == 0)
        return OP_FUNC;
    else if (stricmp(str, "RET") == 0)
        return OP_RET;
    else if (stricmp(str, "WHILE") == 0)
        return OP_WHILE;
    else if (stricmp(str, "VAR") == 0)
        return OP_VAR;
    else if (stricmp(str, "EQ") == 0)
        return OP_EQ;
    else if (stricmp(str, "IS_EE") == 0)
        return OP_IS_EE;
    else if (stricmp(str, "IS_GE") == 0)
        return OP_IS_GE;
    else if (stricmp(str, "IS_BE") == 0)
        return OP_IS_BE;
    else if (stricmp(str, "IS_GT") == 0)
        return OP_IS_GT;
    else if (stricmp(str, "IS_BT") == 0)
        return OP_IS_BT;
    else if (stricmp(str, "IS_NE") == 0)
        return OP_IS_NE;
    else if (stricmp(str, "CALL") == 0)
        return OP_CALL;
    else if (stricmp(str, "ST") == 0)
        return OP_STAT;
    else if (stricmp(str, "PARAM") == 0)
        return OP_PARAM;

    return OP_UNKNOWN;
}

Node* CreateNode(NodeType type, double val, OperType optype, char* varvalue, Tree* tree, Node* ancestor, Node* leftchild, Node* rightchild, int line)
{
    Node* newnode = (Node*) calloc(1, sizeof(Node));

    newnode->optype = optype;
    newnode->val = val;
    newnode->type = type;
    newnode->varvalue = varvalue;
    newnode->tree = tree;
    newnode->ancestor = ancestor;
    newnode->line = line;

    newnode->leftchild = leftchild;
    newnode->rightchild = rightchild;

    return newnode;
}

Node* ReadFromStandart()
{
    Source src = {};

    TextReader("data.txt", &src, "r");
    LinesSeparator(&src, '\n');

    char* anchor = src.Strings[1].ptr;
    double val = 0;

    while (isspace(*anchor))
        anchor += 1;

    Node* currnode = CreateNode(OP_TYPE, 0, OP_STAT, NULL, NULL, NULL, NULL, NULL, 0);
    Node* returnnode = currnode;

    for (int counter = 2; counter < src.nlines; counter++)
    {
        char* curr = src.Strings[counter].ptr;

        while(isspace(*curr))
            curr += 1;

        if (*curr == '}')
        {
            currnode = currnode->ancestor;
            continue;
        }
        else if (*curr == '{')
        {
            continue;
        }
        else
        {
            if (currnode->leftchild)
            {
                if (OperType optype = IsStdOper(curr))
                    currnode->rightchild = CreateOpType(optype);
                else if (sscanf(curr, "%lg", &val) == 1)
                    currnode->rightchild = CreateNum(val);
                else
                    currnode->rightchild = CreateVar(curr);
                currnode = currnode->rightchild;
                continue;
            }
            else
            {
                if (OperType optype = IsStdOper(curr))
                {
                    currnode->leftchild = CreateOpType(optype);
                }
                else if (sscanf(curr, "%lg", &val) == 1)
                    currnode->leftchild = CreateNum(val);
                else
                    currnode->leftchild = CreateVar(curr);
                currnode = currnode->leftchild;
                continue;
            }
        }
    }

    return returnnode;
}

int DataPrint(Node* node)
{
    FILE* data = fopen("data.txt", "w");

    NodePrint(data, node);

    fclose(data);

    return NOERR;
}

int NodePrint(FILE* data, Node* node)
{
    fprintf(data, "{\n");
    ContentPrint(data, node);

    if (node->leftchild)
        NodePrint(data, node->leftchild);

    if (node->rightchild)
        NodePrint(data, node->rightchild);

    fprintf(data, "}\n");

    return NOERR;
}

int ContentPrint(FILE* data, Node* node)
{
    if (node->type == NUM_TYPE)
        fprintf(data, "%lg\n", node->val);

    else if (node->type == VAR_TYPE)
        fprintf(data, "%s\n", node->varvalue);

    else if (node->type == OP_TYPE)
    {
        OpTypePrint(data, node->optype);
        fprintf(data, "\n");
    }

    return NOERR;
}

int CreateAncestor(Node* node, Node* ancestor, Tree* tree)
{
    DBG assert (node != NULL);
    DBG assert (node != NULL);

    node->tree = tree;
    tree->size++;

    if (node->leftchild)
        CreateAncestor(node->leftchild, node, tree);
    if (node->rightchild)
        CreateAncestor(node->rightchild, node, tree);

    node->ancestor = ancestor;

    return NOERR;
}

void SetColor(enum Colors color)
{
    if (color == WHITE)
        txSetConsoleAttr(FOREGROUND_WHITE);
    else if (color == GREEN)
        txSetConsoleAttr(FOREGROUND_GREEN);
    else if (color == RED)
        txSetConsoleAttr(FOREGROUND_RED);
}

int compare(const double a, const double b)
{
    if (fabs(a-b) < EPSILON)
        return 0;
    if ((a-b) > EPSILON)
        return 1;
    if ((a-b) < EPSILON)
        return -1;

    return NOERR;
}
