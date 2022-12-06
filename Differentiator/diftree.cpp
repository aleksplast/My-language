#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <assert.h>

#include "diftree.h"
#include "common.h"

const char* Poison = NULL;

#define TREECHECK   if (int errors = TreeVerr(tree))                                         \
                        DBG TreeGraphDump(tree, errors, __LINE__, __func__, __FILE__);

static int Piccounter = 1;

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

    char picture[100] = "picture";
    sprintf(picture, "pictures\\%d.dot", Piccounter);
    char picturesrc[100] = "picture";
    sprintf(picturesrc, "pictures\\%d.png", Piccounter);

    FILE* pic = fopen(picture, "w");

    fprintf(pic, "strict graph {\n");
    fprintf(pic, "\trankdir = TB\n");

    int counter = 1;
    NodeDump(tree->anchor, &counter, pic);

    fprintf(pic, "}");

    fclose(pic);
    Piccounter += 1;

    char src[100] = "";
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
    else if (node->val)
        fprintf(pic, "%lg", node->val);
    else if (node->varvalue)
        fprintf(pic, "%s", node->varvalue);
    else
        fprintf(pic, "%.0lf", 0);
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

    return NOERR;
}

OperType IsOper(char* str)
{
    if (strcmp(str, "raskrutit'") == 0)
        return OP_MUL;
    else if (strcmp(str, "baff") == 0)
        return OP_ADD;
    else if (strcmp(str, "klif") == 0)
        return OP_DIV;
    else if (strcmp(str, "damag") == 0)
        return OP_SUB;
    else if (strcmp(str, "aptaverny") == 0)
        return OP_POWER;
    else if (strcmp(str, "zdarova") == 0)
        return OP_OPBLOCK;
    else if (strcmp(str, "poka") == 0)
        return OP_CLBLOCK;
    else if (strcmp(str, "podrubai") == 0)
        return OP_OPBRC;
    else if (strcmp(str, "ofai") == 0)
        return OP_CLBRC;
    else if (strcmp(str, "vlad") == 0)
        return OP_OR;
    else if (strcmp(str, "stas") == 0)
        return OP_AND;
    else if (strcmp(str, "vin") == 0)
        return OP_IF;
    else if (strcmp(str, "luz") == 0)
        return OP_ELSE;
    else if (strcmp(str, "katka") == 0)
        return OP_FUNC;
    else if (strcmp(str, "chat") == 0)
        return OP_SEP;
    else if (strcmp(str, "vylet") == 0)
        return OP_RET;
    else if (strcmp(str, "regat\'") == 0)
        return OP_WHILE;
    else if (strcmp(str, "karta") == 0)
        return OP_VAR;
    else if (strcmp(str, "staty") == 0)
        return OP_EQ;
    else if (strcmp(str, "iiiii") == 0)
        return OP_COMMA;

    return OP_UNKNOWN;
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
