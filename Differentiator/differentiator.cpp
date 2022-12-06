#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include <locale.h>

#include "TXLib.h"
#include "differentiator.h"

//TO DO LIST

char Econst[2] = "e";
char Xconst[2] = "x";
char Aconst[2] = "A";
const int STRSIZE = 50;
const int CHARSTR = 2;
static bool CHANGE = true;
const double EPSILON = 1e-10;
static bool PrintTeX = true;
const int RECOMENDSIZE = 50;

#define SIMPTEXSTART(node)  if (PrintTeX)                                                                       \
                            {                                                                                   \
                                fprintf(out, "%s\n\n", phrases->Strings[rand()%(phrases->nlines)].ptr);     \
                                TeXPrint(node, Aconst, out);                                                    \
                            }

#define SIMPTEXEND(node)    if (PrintTeX)                       \
                            {                                   \
                                fprintf(out, " = ");            \
                                TeXPrint(node, Aconst, out);    \
                                fprintf(out, "\n\n");           \
                            }

int DifferentiatorMain(char* input)
{
    setlocale(LC_ALL,"Rus");

    char diffphrases[STRSIZE] = "DiffBundles.txt";
    char simpphrases[STRSIZE] = "SimplifyBundles.txt";
    char parameter[STRSIZE] = "Parameters.txt";

    Phrases inputdata = {};
    Input data = ManageInput(&inputdata, input);

    Tree datatree = DataParser(data.equation);

    Phrases diffbundles = {};
    GetPhrases(&diffbundles, diffphrases);

    Phrases simpbundles = {};
    GetPhrases(&simpbundles, simpphrases);

    Phrases parameters = {};
    Variable* variables = GetParameters(&parameters, parameter);

    TreeGraphDump(&datatree, 0, __LINE__, __func__, __FILE__);

    FILE* out = fopen("data\\output.txt", "w");

    PrepareTeXFile(out);
    TeXPrint(datatree.anchor, Aconst, out);

    Tree difftree = DiffFuncOnce(&datatree, data.variable, &diffbundles, &simpbundles, out);

    TangentEquation(&datatree, &difftree, data.varvalue, out);

    CreateGraphic(&datatree, data.leftborder, data.rightborder, out);

    FullInaccuracy(&datatree, variables, parameters.nlines, &diffbundles, &simpbundles, out);

    MaclaurinSeries(&datatree, &difftree, data.order, data.variable, &diffbundles, &simpbundles, out);

    TeXClose(out);

    fclose(out);

    free(parameters.Strings);
    free(diffbundles.Strings);
    free(simpbundles.Strings);

    TreeDetor(&datatree);
    TreeDetor(&difftree);

    system("pdflatex  -output-directory=data data\\output.txt > data\\tex_log");
    system("del data\\output.log");
    system("del data\\output.aux");
    system("del data\\tex_log");
    system("start data\\output.pdf");

    return NOERR;
}

Input ManageInput(Phrases* phrases, char* input)
{
    const char* mode = "r";
    const char end = '\n';

    TextReader(input, phrases, mode);

    LinesSeparator(phrases, end);

    Input data = {};
    data.variable = (char*) calloc(STRSIZE, sizeof(char));
    data.equation = phrases->Strings[0].ptr;

    sscanf(phrases->Strings[2].ptr, "%s %lg", data.variable, &data.varvalue);
    sscanf(phrases->Strings[3].ptr, "%lg %lg", &data.leftborder, &data.rightborder);
    sscanf(phrases->Strings[4].ptr, "%d", &data.order);

    return data;
}

int GetPhrases(Phrases* phrases, char* input)
{
    const char* mode = "r";
    const char end = '\n';

    TextReader(input, phrases, mode);

    LinesSeparator(phrases, end);

    return NOERR;
}

Variable* GetParameters(Phrases* phrases, char* input)
{
    const char* mode = "r";
    const char end = '\n';

    TextReader(input, phrases, mode);

    LinesSeparator(phrases, end);

    Variable* variables = (Variable*) calloc(phrases->nlines, sizeof(Variable));

    for (int i = 0; i < phrases->nlines; i++)
        sscanf(phrases->Strings[i].ptr, "%s %lg %lg", variables[i].name, &variables[i].val, &variables[i].inaccuracy);

    return variables;
}

OperType IsOper(char* str)
{
    if (strcmp(str, "*") == 0)
        return OP_MUL;
    else if (strcmp(str, "+") == 0)
        return OP_ADD;
    else if (strcmp(str, "/") == 0)
        return OP_DIV;
    else if (strcmp(str, "-") == 0)
        return OP_SUB;
    else if (strcmp(str, "sin") == 0)
        return OP_SIN;
    else if (strcmp(str, "cos") == 0)
        return OP_COS;
    else if (strcmp(str, "^") == 0)
        return OP_POWER;
    else if (strcmp(str, "tg") == 0)
        return OP_TG;
    else if (strcmp(str, "log") == 0)
        return OP_LOG;
    else if (strcmp(str, "ln") == 0)
        return OP_LN;
    else if (strcmp(str, "ctg") == 0)
        return OP_CTG;
    else if (strcmp(str, "sqrt") == 0)
        return OP_SQRT;
    else if (strcmp(str, "arcsin") == 0)
        return OP_ARCSIN;
    else if (strcmp(str, "arccos") == 0)
        return OP_ARCCOS;
    else if (strcmp(str, "arctg") == 0)
        return OP_ARCTG;
    else if (strcmp(str, "arcctg") == 0)
        return OP_ARCCTG;
    else if (strcmp(str, "sh") == 0)
        return OP_SH;
    else if (strcmp(str, "ch") == 0)
        return OP_CH;

    return OP_UNKNOWN;
}

bool IsOneArg(Node* node)
{
    if (node->type == OP_TYPE)
    {
        if (node->optype == OP_COS || node->optype == OP_SIN || node->optype == OP_TG || node->optype == OP_CTG ||
            node->optype == OP_LN || node->optype == OP_SQRT || node->optype == OP_ARCSIN || node->optype == OP_ARCCOS
            || node->optype == OP_ARCTG || node->optype == OP_ARCCTG || node->optype == OP_CH || node->optype == OP_SH)
            return true;
        else
            return false;
    }
    else
        return false;

    return false;
}

Tree DataParser(char* input)
{
    Tree difftree = {};

    char log[STRSIZE] = "pictures\\graphlog.htm";

    Node** nodes = LexicAnalizer(input);

    Node* node = GetG(nodes);

    TreeCtor(&difftree, NUM_TYPE, 0, OP_UNKNOWN, NULL, log);
    difftree.anchor = node;
    TreeGraphDump(&difftree, 0, __LINE__, __func__, __FILE__);

    CreateAncestor(difftree.anchor, NULL, &difftree);

    return difftree;
}

int FillCurrNode(Node* currnode, char* input)
{
    double val = 0;
    char* str = (char*) calloc(STRSIZE, sizeof(char));
    int len = 0;

    if (sscanf(input, "%lg%n", &val, &len) == 1)
    {
        currnode->type = NUM_TYPE;
        currnode->val = val;

        return len - 1;
    }
    else if (sscanf(input, "%[^()]%n", str, &len) == 1)
    {
        if (OperType optype = IsOper(str))
        {
            currnode->type = OP_TYPE;
            currnode->optype = optype;
        }
        else
        {
            currnode->type = VAR_TYPE;
            currnode->varvalue = str;
        }

        return len - 1;
    }

    return len;
}

Node* NodeCopy(Node* node)
{
    DBG assert(node != NULL);

    Node* newnode = (Node*) calloc(1, sizeof(Node));

    newnode->optype = node->optype;
    newnode->val = node->val;
    newnode->type = node->type;
    newnode->varvalue = node->varvalue;

    if (node->leftchild)
        newnode->leftchild = NodeCopy(node->leftchild);
    if (node->rightchild)
        newnode->rightchild = NodeCopy(node->rightchild);

    return newnode;
}

Node* CreateNode(NodeType type, double val, OperType optype, char* varvalue, Tree* tree, Node* ancestor, Node* leftchild, Node* rightchild)
{
    Node* newnode = (Node*) calloc(1, sizeof(Node));

    newnode->optype = optype;
    newnode->val = val;
    newnode->type = type;
    newnode->varvalue = varvalue;
    newnode->tree = tree;
    newnode->ancestor = ancestor;

    newnode->leftchild = leftchild;
    newnode->rightchild = rightchild;

    return newnode;
}

Node* DiffNode(Node* node, Phrases* phrases, char* var, FILE* out)
{
    DBG assert(node != NULL);

    Node* newnode = node;

    switch (node->type)
    {
        case NUM_TYPE:
            newnode = CREATENUM(0);
            break;
        case VAR_TYPE:
            if (strcmp(node->varvalue, var) == 0)
                newnode = CREATENUM(1);
            else
                newnode = CREATENUM(0);
            break;
        case OP_TYPE:
            switch (node->optype)
            {
                case OP_ADD:
                    newnode = ADD(DL, DR);
                    break;
                case OP_SUB:
                    newnode = SUB(DL, DR);
                    break;
                case OP_MUL:
                    newnode = ADD(MUL(DL, CR), MUL(CL, DR));
                    break;
                case OP_DIV:
                    newnode = DIV(SUB(MUL(DL, CR), MUL(CL, DR)), POWER(CR, CREATENUM(2)));
                    break;
                case OP_SIN:
                    newnode = MUL(COS(NULL, CR), DR);
                    break;
                case OP_COS:
                    newnode = MUL(MUL(SIN(NULL, CR), CREATENUM(-1)), DR);
                    break;
                case OP_POWER:
                    newnode = DiffPower(node, phrases, var, out);
                    break;
                case OP_LOG:
                    newnode = DiffLog(node, phrases, var, out);
                    break;
                case OP_LN:
                    newnode = MUL(DIV(CREATENUM(1), CR), DR);
                    break;
                case OP_TG:
                    newnode = MUL(DIV(CREATENUM(1), POWER(COS(NULL, CR), CREATENUM(2))), DR);
                    break;
                case OP_CTG:
                    newnode = MUL(MUL(CREATENUM(-1), DIV(CREATENUM(1), POWER(SIN(NULL, CR), CREATENUM(2)))), DR);
                    break;
                case OP_SQRT:
                    newnode = MUL(DIV(CREATENUM(1), MUL(CREATENUM(2), SQRT(NULL, CR))), DR);
                    break;
                case OP_ARCSIN:
                    newnode = MUL(DIV(CREATENUM(1), SQRT(NULL, SUB(CREATENUM(1), POWER(CR, CREATENUM(2))))), DR);
                    break;
                case OP_ARCCOS:
                    newnode = MUL(MUL(CREATENUM(-1), DIV(CREATENUM(1), SQRT(NULL, SUB(CREATENUM(1), POWER(CR, CREATENUM(2)))))), DR);
                    break;
                case OP_ARCTG:
                    newnode = MUL(DIV(CREATENUM(1), ADD(CREATENUM(1), POWER(CR, CREATENUM(2)))), DR);
                    break;
                case OP_ARCCTG:
                    newnode = MUL(MUL(CREATENUM(-1), DIV(CREATENUM(1), ADD(CREATENUM(1), POWER(CR, CREATENUM(2))))), DR);
                    break;
                case OP_SH:
                    newnode = MUL(CH(NULL, CR), DR);
                    break;
                case OP_CH:
                    newnode = MUL(SH(NULL, CR), DR);
                    break;
                case OP_UNKNOWN:
                    return node;
                default:
                    return node;
            }
        case UNKNOWN_TYPE:
            break;
        default:
            break;
    }

    CreateAncestor(newnode, newnode, node->tree);

    if (PrintTeX)
    {
        fprintf(out, "%s\n\n", phrases->Strings[rand()%(phrases->nlines)].ptr);
        fprintf(out, "(");
        TeXPrint(node, Aconst, out);
        fprintf(out, ")$'$");
        fprintf(out, " = ");
        TeXPrint(newnode, Aconst, out);
        fprintf(out, "\n\n");
    }

    return newnode;
}

Node* DiffPower(Node* node, Phrases* phrases, char* var, FILE* out)
{
    DBG assert(node != NULL);

    Node* searchright = NULL;
    Node* searchleft = NULL;

    if (node->leftchild)
        searchleft = TreeDepthSearch(node->leftchild, var);
    if (node->rightchild)
        searchright = TreeDepthSearch(node->rightchild, var);

    if (searchleft && searchright)
        return MUL(POWER(CREATEVAR(Econst), MUL(LN(NULL, CL), CR)), DiffNode(MUL(LN(NULL, CL), CR), phrases, var, out));
    else if (searchleft)
        return MUL(MUL(CR, POWER(CL, SUB(CR, CREATENUM(1)))), DL);
    else if (searchright)
        return MUL(MUL(POWER(CL,CR), LN(NULL, CL)), DR);
    else
        return CREATENUM(0);

    return node;
}

Node* DiffLog(Node* node, Phrases* phrases, char* var, FILE* out)
{
    DBG assert(node != NULL);

    Node* searchleft = TreeDepthSearch(node->leftchild, var);
    Node* searchright = TreeDepthSearch(node->rightchild, var);

    if (searchleft && searchright)
        return DiffNode(DIV(LN(NULL, CR), LN(NULL, CL)), phrases, var, out);
    else if (searchleft)
        return DiffNode(DIV(LN(NULL, CR), LN(NULL, CL)), phrases, var, out);
    else if (searchright)
        return MUL(DIV(CREATENUM(1), MUL(LN(NULL, CL), CR)), DR);
    else
        return CREATENUM(0);
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

int SimplifyFunc(Tree* tree, Phrases* phrases, FILE* out)
{
    DBG assert(tree != NULL);

    CHANGE = true;

    while (CHANGE)
    {
        SimplifyConstants(tree->anchor, phrases, out);

        DeleteMeaninglessNodes(tree->anchor, phrases, out);
    }

    return NOERR;
}

int SimplifyConstants(Node* node, Phrases* phrases, FILE* out)
{
    DBG assert(node != NULL);

    CHANGE = false;

    if (node->leftchild)
        SimplifyConstants(node->leftchild, phrases, out);

    if (node->rightchild)
        SimplifyConstants(node->rightchild, phrases, out);

    SimplifyConstantNode(node, phrases, out);

    return NOERR;
}

int SimplifyConstantNode(Node* node, Phrases* phrases, FILE* out)
{
    DBG assert(node != NULL);

    if (node->type != OP_TYPE)
        return NOERR;

    if (!node->leftchild)
        return NOERR;

    if (node->leftchild->type != NUM_TYPE || node->rightchild->type != NUM_TYPE)
        return NOERR;

    if (node->optype == OP_LOG || node->optype == OP_DIV)
        return NOERR;

    if (PrintTeX)
    {
        fprintf(out, "%s\n\n", phrases->Strings[rand()%(phrases->nlines)].ptr);
        TeXPrint(node, Aconst, out);
    }

    if (node->optype == OP_SUB)
        node->val = node->leftchild->val - node->rightchild->val;

    else if (node->optype == OP_MUL)
        node->val = node->leftchild->val * node->rightchild->val;

    else if (node->optype == OP_ADD)
        node->val = node->leftchild->val + node->rightchild->val;

    else if (node->optype == OP_POWER)
        node->val = pow(node->leftchild->val, node->rightchild->val);

    node->type = NUM_TYPE;
    node->varvalue = NULL;
    node->optype = OP_UNKNOWN;
    node->leftchild = NULL;
    node->rightchild = NULL;
    CHANGE = true;

    if (PrintTeX)
    {
        fprintf(out, " = ");
        TeXPrint(node, Aconst, out);
        fprintf(out, "\n\n");
    }

    return NOERR;
}

int DeleteMeaninglessNodes(Node* node, Phrases* phrases, FILE* out)
{
    DBG assert(node != NULL);

    if (node->leftchild)
        DeleteMeaninglessNodes(node->leftchild, phrases, out);
    if (node->rightchild)
        DeleteMeaninglessNodes(node->rightchild, phrases, out);

    CheckNDeleteNode(node, phrases, out);

    return NOERR;
}

int CheckNDeleteNode(Node* node, Phrases* phrases, FILE* out)
{
    DBG assert (node != NULL);

    if (node->type != OP_TYPE)
        return NOERR;
    if (!node->leftchild || !node->rightchild)
        return NOERR;

    if (node->rightchild->type == NUM_TYPE && (node->optype == OP_ADD || node->optype == OP_MUL))
    {
        Node* temp = node->rightchild;
        node->rightchild = node->leftchild;
        node->leftchild = temp;
    }

    if (IS_OP(OP_MUL) && IS_LVAL(1))
    {
        SIMPTEXSTART(node)
        ChangeIntoChild(node, RIGHT);
        SIMPTEXEND(node->rightchild)
    }

    else if (IS_OP(OP_MUL) && IS_LVAL(0))
    {
        SIMPTEXSTART(node)
        ChangeNodeIntoNum(node, 0);
        SIMPTEXEND(node)
    }

    else if (IS_OP(OP_ADD) && IS_LVAL(0))
    {
        SIMPTEXSTART(node)
        ChangeIntoChild(node, RIGHT);
        SIMPTEXEND(node->rightchild)
    }

    else if (IS_OP(OP_SUB) && IS_LVAL(0))
    {
        SIMPTEXSTART(node)
        ChangeIntoChild(node, RIGHT);
        SIMPTEXEND(node->rightchild)
    }

    else if (IS_OP(OP_POWER) && IS_RVAL(0))
    {
        SIMPTEXSTART(node)
        ChangeNodeIntoNum(node, 1);
        SIMPTEXEND(node)
    }

    else if (IS_OP(OP_POWER) && IS_RVAL(1))
    {
        SIMPTEXSTART(node)
        ChangeIntoChild(node, LEFT);
        SIMPTEXEND(node->leftchild)
    }

    else if (IS_OP(OP_DIV) && IS_LVAL(0))
    {
        SIMPTEXSTART(node)
        ChangeNodeIntoNum(node, 0);
        SIMPTEXEND(node)
    }

    return NOERR;
}

int ChangeIntoChild(Node* node, Side side)
{
    if (node == node->tree->anchor)
        return DeleteAnchor(node, side);

    Node* child = NULL;
    Node* killchild = NULL;

    if (side == RIGHT)
    {
        child = node->rightchild;
        killchild = node->leftchild;
    }
    else
    {
        child = node->leftchild;
        killchild = node->rightchild;
    }

    if (node == node->ancestor->leftchild)
    {
        node->ancestor->leftchild = child;
        child->ancestor = node->ancestor;
        node->tree->size--;
        NodeDetor(killchild);
        free(node);
    }
    else if (node == node->ancestor->rightchild)
    {
        node->ancestor->rightchild = child;
        child->ancestor = node->ancestor;
        node->tree->size--;
        NodeDetor(killchild);
        free(node);
    }
    CHANGE = true;

    return NOERR;
}

int ChangeNodeIntoNum(Node* node, double val)
{
    DBG assert(node != NULL);

    node->type = NUM_TYPE;
    node->val = val;
    node->varvalue = NULL;
    node->optype = OP_UNKNOWN;

    if (node->rightchild)
        NodeDetor(node->rightchild);
    if (node->leftchild)
        NodeDetor(node->leftchild);

    node->leftchild = NULL;
    node->rightchild = NULL;

    if (node == node->tree->anchor)
        node->ancestor = NULL;

    CHANGE = true;

    return NOERR;
}

int DeleteAnchor(Node* node, Side side)
{
    DBG assert(node != NULL);

    if (side == RIGHT)
    {
        node->tree->anchor = node->rightchild;
        free(node);
        NodeDetor(node->leftchild);
    }
    else
    {
        node->tree->anchor = node->leftchild;
        free(node);
        NodeDetor(node->rightchild);
    }

    return NOERR;
}

double CalculateNode(Node* node, double x)
{
    DBG assert(node != NULL);

    switch (node->type)
    {
        case NUM_TYPE:
            return node->val;
        case VAR_TYPE:
            switch(*(node->varvalue))
            {
                case 'x':
                    return x;
                case 'e':
                    return 1e1;
                default:
                    return 0;
            }
        case OP_TYPE:
            switch (node->optype)
            {
                case OP_ADD:
                    return CalculateNode(node->leftchild, x) + CalculateNode(node->rightchild,x);
                case OP_MUL:
                    return CalculateNode(node->leftchild, x) * CalculateNode(node->rightchild,x);
                case OP_DIV:
                    return CalculateNode(node->leftchild, x) / CalculateNode(node->rightchild,x);
                case OP_SUB:
                    return CalculateNode(node->leftchild, x) - CalculateNode(node->rightchild,x);
                case OP_SIN:
                    return sin(CalculateNode(node->rightchild, x));
                case OP_COS:
                    return cos(CalculateNode(node->rightchild, x));
                case OP_TG:
                    return tan(CalculateNode(node->rightchild,x));
                case OP_POWER:
                    return pow(CalculateNode(node->leftchild, x), CalculateNode(node->rightchild,x));
                case OP_LOG:
                    return (log(CalculateNode(node->rightchild, x)))/(log(CalculateNode(node->leftchild,x)));
                case OP_LN:
                    return log(CalculateNode(node->rightchild,x));
                case OP_CTG:
                    return 1/(tan(CalculateNode(node->rightchild,x)));
                case OP_SQRT:
                    return sqrt(CalculateNode(node->rightchild,x));
                case OP_ARCCOS:
                    return acos(CalculateNode(node->rightchild,x));
                case OP_ARCSIN:
                    return asin(CalculateNode(node->rightchild,x));
                case OP_ARCTG:
                    return atan(CalculateNode(node->rightchild,x));
                case OP_ARCCTG:
                    return txPI - atan(CalculateNode(node->rightchild,x));
                case OP_SH:
                    return sinh(CalculateNode(node->rightchild,x));
                case OP_CH:
                    return cosh(CalculateNode(node->rightchild,x));
                case OP_UNKNOWN:
                    return 0;
                default:
                    return 0;
            }
        case UNKNOWN_TYPE:
            return 0;
        default:
            return 0;
    }
}

int SubTreeSize(Node* node)
{
    int size = 0;

    CountSubTreeSize(node, &size);

    return size;
}

int CountSubTreeSize(Node* node, int* size)
{
    *size += 1;
    int origsize = *size;
    int size1 = 0;
    int size2 = 0;

    if (node->leftchild)
        size1 = CountSubTreeSize(node->leftchild, size) - origsize;
    if (node->rightchild)
        size2 = CountSubTreeSize(node->rightchild, size) - size1;

    if (node->optype == OP_DIV)
    {
        if (size1 > size2)
            *size -= size2;
        else
            *size -= size1;
    }

    return *size;
}

int PrepareCalcNode(Node* node, Variable* variables, long varsize)
{
    if (node->leftchild)
        PrepareCalcNode(node->leftchild, variables, varsize);
    if (node->rightchild)
        PrepareCalcNode(node->rightchild, variables, varsize);

    if (node->type != VAR_TYPE)
        return NOERR;

    for (int i = 0; i < varsize; i++)
    {
        if (strcmp(variables[i].name, node->varvalue) == 0)
        {
            ChangeNodeIntoNum(node, variables[i].val);
            break;
        }
    }

    return NOERR;
}

Tree DiffFuncOnce(Tree* datatree, char* var, Phrases* diffphrases, Phrases* simpphrases, FILE* out)
{
    Tree difftree = {};
    char difftreelog[STRSIZE] = "pictures\\difftreelog.htm";
    TreeCtor(&difftree, UNKNOWN_TYPE, 0, OP_UNKNOWN, NULL, difftreelog);
    if (PrintTeX)
        fprintf(out, "\n\nПродифференцируем же эту функцию во имя Бога-Машины\n\n");

    Node* newanchor = DiffNode(datatree->anchor, diffphrases, var, out);

    difftree.anchor = newanchor;
    difftree.anchor->tree = &difftree;

    CreateAncestor(difftree.anchor, NULL, &difftree);
    SimplifyFunc(&difftree, simpphrases, out);

    if (PrintTeX)
    {
        fprintf(out, "Итак, с благословлением Омниссии мы получили:\n\n");
        fprintf(out, "(");
        TeXPrint(datatree->anchor, Aconst, out);
        fprintf(out, ")' = ");
        TeXPrint(difftree.anchor, Aconst, out);
        fprintf(out, "\n\n");
    }

    TreeGraphDump(&difftree, 0, __LINE__, __func__, __FILE__);

    return difftree;
}

double FullInaccuracy(Tree* datatree, Variable* variables, long varsize, Phrases* diffphrases, Phrases* simpphrases, FILE* out)
{
    double inacurracy = 0;
    PrintTeX = false;
    for (int i = 1; i < varsize; i++)
    {
        Tree difftree = DiffFuncOnce(datatree, variables[i].name, diffphrases, simpphrases, out);
        TreeGraphDump(&difftree, 0, __LINE__, __func__, __FILE__);
        PrepareCalcNode(difftree.anchor, variables, varsize);

        TreeGraphDump(&difftree, 0, __LINE__, __func__, __FILE__);

        double partinac = CalculateNode(difftree.anchor, variables[i].val);

        inacurracy += partinac * partinac * variables[i].inaccuracy * variables[i].inaccuracy;
        NodeDetor(difftree.anchor);
    }

    inacurracy = sqrt(inacurracy);

    fprintf(out, "2. Полная погрешность данного измерения составляет $%lg$\n\n", inacurracy);

    return inacurracy;
}

int TangentEquation(Tree* datatree, Tree* difftree, double x, FILE* out)
{
    fprintf(out, "\\section{Другая полезная информация}\n\n");
    fprintf(out, "1. Для проведения экстерминатуса очевидно необходимо знать уравнение касательной в точке $x = %0.2lg$, так еретиков останется меньше\n\n", x);

    double derval = CalculateNode(difftree->anchor, x);
    double val = CalculateNode(datatree->anchor, x);

    fprintf(out, "$$y = %.2lgx + %.2lg$$\n\n", derval, val+derval*x);

    return NOERR;
}

int MaclaurinSeries(Tree* datatree, Tree* difftree, int order, char* var, Phrases* diffphrases, Phrases* simpphrases, FILE* out)
{
    PrintTeX = false;

    char serieslog[STRSIZE] = "pictures\\serieslog.htm";

    fprintf(out, "\n\n 3. В М36.567 Верховный совет Терры признал Маклорена еретиком, но у нас свои законы, разложим же эту функцию по его формуле\n\n");
    Tree seriestree = {};
    TreeCtor(&seriestree, NUM_TYPE, CalculateNode(datatree->anchor, 0), OP_UNKNOWN, NULL, serieslog);

    Node* node = (Node*) calloc(1, sizeof(node));
    node->tree = &seriestree;
    node = ADD(NULL, MUL(CREATENUM(CalculateNode(difftree->anchor, 0)), CREATEVAR(Xconst)));
    node->leftchild = seriestree.anchor;
    seriestree.anchor = node;
    int counter = 2;

    for (counter = 2; counter <= order; counter++)
    {
        Node* newanchor = DiffNode(difftree->anchor, diffphrases, var, out);

        difftree->anchor = newanchor;
        difftree->anchor->tree = difftree;

        CreateAncestor(difftree->anchor, NULL, difftree);
        SimplifyFunc(difftree, simpphrases, out);

        TreeGraphDump(difftree, 0, __LINE__, __func__, __FILE__);

        node = ADD(NULL, MUL(DIV(CREATENUM(CalculateNode(difftree->anchor, 0)), CREATENUM(Factorial(counter))), POWER(CREATEVAR(Xconst), CREATENUM(counter))));
        node->leftchild = seriestree.anchor;
        seriestree.anchor = node;
        seriestree.anchor->tree = &seriestree;
        TreeGraphDump(&seriestree, 0, __LINE__, __func__, __FILE__);
    }

    CreateAncestor(seriestree.anchor, NULL, &seriestree);
    SimplifyFunc(&seriestree, simpphrases, out);
    //TreeGraphDump(&seriestree, 0, __LINE__, __func__, __FILE__);

    TeXPrint(datatree->anchor, Aconst, out);
    fprintf(out, " $=$ ");
    TeXPrint(seriestree.anchor, Aconst, out);
    fprintf(out, " $ + o({x}^{%d}) $ \n\n", counter - 1);

    TreeDetor(&seriestree);

    return NOERR;
}

int CreateGraphic(Tree* datatree, double left, double right, FILE* out)
{
    FILE* graphic = fopen("graphic.txt", "w");

    for (double x = left; x < right; x += 0.02)
        fprintf(graphic, "%lg %lg\n", x, CalculateNode(datatree->anchor, x));

    fclose(graphic);

    system("py C:\\Users\\USER\\Documents\\Differentiator\\graphic.py");

    fprintf(out,    "\\begin{figure}[h!]\n"
                    "\\begin{center}\n"
                    "\\includegraphics{graphic}\n"
                    "\\end{center}\n"
                    "\\caption{График количества еретиков}\n"
                    "\\end{figure}\n\n");

    return NOERR;
}

int Priority(Node* node)
{
    if (node->type != OP_TYPE)
        return 0;

    if (node->optype == OP_ADD || node->optype == OP_SUB)
        return 1;
    else if (node->optype == OP_MUL || node->optype == OP_DIV)
        return 2;
    else
        return 3;

    return NOERR;
}

int Factorial(int n)
{
    if (n < 1)
        return NOERR;
    int fact = 1;

    for (int counter = 1; counter <= n; counter++)
        fact *= counter;

    return fact;
}

int TeXPrint(Node* node, char* repl, FILE* out)
{
    char replace = *repl;
    int sizeleft = 0;
    int sizeright = 0;
    Node* right = node->rightchild;
    Node* left = node->leftchild;

    if (node->leftchild)
        sizeleft = SubTreeSize(node->leftchild);
    if (node->rightchild)
        sizeright = SubTreeSize(node->rightchild);

    if (sizeleft + sizeright + 1 <= RECOMENDSIZE)
    {
        fprintf(out, " $ ");
        TeXNodePrint(node, node, out);
        fprintf(out, " $ ");
    }

    if (sizeleft > RECOMENDSIZE && sizeright > RECOMENDSIZE)
    {
        node->leftchild =  CREATEVAR(repl);
        *repl = (char)(*repl +  1);
        node->rightchild = CREATEVAR(repl);
        fprintf(out, " $ ");
        TeXNodePrint(node, node, out);
        fprintf(out, "\n\n$Где %c = $ ", replace);
        *repl = (char)(*repl +  1);
        TeXPrint(right, repl, out);
        fprintf(out, "\n\n%c = ", replace - 1);
        *repl = (char)(*repl +  1);
        TeXPrint(left, repl, out);
    }
    else if (sizeleft + 1 > RECOMENDSIZE)
    {
        node->leftchild =  CREATEVAR(repl);
        fprintf(out, " $ ");
        TeXNodePrint(node, node, out);
        fprintf(out, " $ ");
        fprintf(out, "\n\nГде $%c = $ ", replace);
        *repl = (char)(*repl +  1);
        TeXPrint(left, repl, out);
    }
    else if (sizeright + 1 > RECOMENDSIZE)
    {
        node->rightchild = CREATEVAR(repl);
        fprintf(out, " $ ");
        TeXNodePrint(node, node, out);
        *repl =(char)(*repl +  1);
        fprintf(out, " $\n\nГде $ %c = $ ", replace);
        TeXPrint(right, repl, out);
    }

    node->rightchild = right;
    node->leftchild = left;

    return NOERR;
}

int TeXNodePrint(Node* startnode, Node* node, FILE* out)
{
    TeXDataStartPrint(startnode, node, out);

    if (node->leftchild && !IsOneArg(node))
        TeXNodePrint(startnode, node->leftchild, out);

    TeXDataPrint(node, out);

    if (node->rightchild)
        TeXNodePrint(startnode, node->rightchild, out);

    TeXDataEndPrint(startnode, node, out);

    return NOERR;
}

int TeXDataStartPrint(Node* startnode, Node* node, FILE* out)
{
    if (node == startnode && node->optype != OP_DIV)
        return NOERR;

    switch (node->type)
    {
        case NUM_TYPE:
            break;
        case VAR_TYPE:
            break;
        case OP_TYPE:
        {
            if (node->optype == OP_DIV)
                fprintf(out, "\\frac{");
            else if (node->optype >= 5)
                break;
            else if (node->ancestor && Priority(node) >= Priority(node->ancestor) && Priority(node) > 1)
                break;
            else if (node->ancestor && node->optype == node->ancestor->optype)
                break;
            else if (node->ancestor && node->ancestor->optype >= 5)
                break;
            else
                fprintf(out, "(");
            break;
        }
        case UNKNOWN_TYPE:
            break;
        default:
            break;
    }

    return NOERR;
}

int TeXDataPrint(Node* node, FILE* out)
{
    switch (node->type)
    {
        case NUM_TYPE:
            fprintf(out, "{%.0lf}", node->val);
            break;
        case VAR_TYPE:
            fprintf(out, "{%s}", node->varvalue);
            break;
        case OP_TYPE:
            switch (node->optype)
            {
                case OP_ADD:
                    fprintf(out, "+");
                    break;
                case OP_SUB:
                    fprintf(out, "-");
                    break;
                case OP_MUL:
                    fprintf(out, "\\cdot");
                    break;
                case OP_DIV:
                    fprintf(out, "}{");
                    break;
                case OP_COS:
                    fprintf(out, "\\cos(");
                    break;
                case OP_SIN:
                    fprintf(out, "\\sin(");
                    break;
                case OP_POWER:
                    fprintf(out, "^{");
                    break;
                case OP_LN:
                    fprintf(out, "\\ln(");
                    break;
                case OP_LOG:
                    fprintf(out, "\\log(");
                    break;
                case OP_TG:
                    fprintf(out, "\\tg(");
                    break;
                case OP_CTG:
                    fprintf(out, "\\ctg(");
                    break;
                case OP_SQRT:
                    fprintf(out, "\\sqrt{");
                    break;
                case OP_ARCSIN:
                    fprintf(out, "\\arcsin(");
                    break;
                case OP_ARCCOS:
                    fprintf(out, "\\arccos(");
                    break;
                case OP_ARCTG:
                    fprintf(out, "\\arctg(");
                    break;
                case OP_ARCCTG:
                    fprintf(out, "\\arcctg(");
                    break;
                case OP_SH:
                    fprintf(out, "\\sinh(");
                    break;
                case OP_CH:
                    fprintf(out, "\\cosh(");
                    break;
                case OP_UNKNOWN:
                    break;
                default:
                    break;
            }
            break;
        case UNKNOWN_TYPE:
            break;
        default:
            break;
    }

    return NOERR;
}

int TeXDataEndPrint(Node* startnode, Node* node, FILE* out)
{
    if (node == startnode && !IsOneArg(node) && node->optype != OP_POWER && node->optype != OP_DIV)
        return NOERR;

    switch (node->type)
    {
        case NUM_TYPE:
            break;
        case VAR_TYPE:
            break;
        case OP_TYPE:
        {
            if (node->optype == OP_DIV || node->optype == OP_POWER || node->optype == OP_SQRT)
                fprintf(out, "}");
            else if (node->optype >= 5)
                fprintf(out, ")");
            else if (node->ancestor && Priority(node) >= Priority(node->ancestor) && Priority(node) > 1)
                break;
            else if (node->ancestor && node->optype == node->ancestor->optype)
                break;
            else if (node->ancestor && node->ancestor->optype >= 5)
                break;
            else
                fprintf(out, ")");
            break;
        }
        case UNKNOWN_TYPE:
            break;
        default:
            break;
    }

    return NOERR;
}

int PrepareTeXFile(FILE* out)
{
    fprintf(out,    "\\documentclass[a4paper,12pt]{article}                    \n"
                    "\\usepackage[a4paper,top=1.3cm,bottom=2cm,left=1.5cm,right=1.5cm,marginparwidth=0.75cm]{geometry}\n"
                    "\\usepackage{cmap}                                                          \n"
                    "\\usepackage{mathtext}                                                      \n"
                    "\\usepackage[T2A]{fontenc}                                                  \n"
                    "\\usepackage[cp1251]{inputenc}                                              \n"
                    "\\usepackage[english,russian]{babel}                                        \n"
                    "\\usepackage{multirow}                                                      \n"
                    "                                                                            \n"
                    "\\usepackage{graphicx}                                                      \n"
                    "                                                                            \n"
                    "\\usepackage{wrapfig}                                                       \n"
                    "\\usepackage{tabularx}                                                      \n"
                    "                                                                            \n"
                    "\\usepackage{amsmath,amsfonts,amssymb,amsthm,mathtools}                     \n"
                    "                                                                            \n"
                    "\\mathtoolsset{showonlyrefs=true}                                           \n"
                    "                                                                            \n"
                    "\\usepackage{mathrsfs}                                                      \n"
                    "                                                                            \n"
                    "\\DeclareMathOperator{\\sgn}{\\mathop{sgn}}                                 \n"
                    "                                                                            \n"
                    "                                                                            \n"
                    "\\date{\\today}                                                             \n"
                    "                                                                            \n"
                    "\\begin{document}                                                           \n"
                    "                                                                            \n"
                    "\\begin{titlepage}                                                          \n"
                    "	\\begin{center}                                                         \n"
                    "		{                                                                   \n"
                    "        \\large МОСКОВСКИЙ ФИЗИКО-ТЕХНИЧЕСКИЙ ИНСТИТУТ}                     \n"
                    "	\\end{center}                                                           \n"
                    "	\\begin{center}                                                         \n"
                    "		{\\large Физтех-школа радиотехники и компьютерных технологий}       \n"
                    "	\\end{center}                                                           \n"
                    "                                                                            \n"
                    "                                                                            \n"
                    "	\\vspace{4.5cm}                                                         \n"
                    "	{\\huge                                                                 \n"
                    "		\\begin{center}                                                     \n"
                    "			{\\bf Дифференцирование функции по заветам Механикус}\\         \n"
                    "		\\end{center}                                                       \n"
                    "	}                                                                       \n"
                    "	\\vspace{2cm}                                                           \n"
                    "	\\begin{flushright}                                                     \n"
                    "		{\\LARGE АВТОР:\\ Магос Техникус Александр Пластинин \\ \\\\       \n"
                    "			\\vspace{0.2cm}}                                                \n"
                    "	\\end{flushright}                                                       \n"
                    "	\\vspace{8cm}                                                           \n"
                    "\\end{titlepage}            \n"
                    "\n");

    fprintf(out, "\\section{Введение}\n"
            "Третье тысячелетие.\n\n"
            "Уже более трех веков Матан недвижим на Золотом Троне Наук.\n\n"
            "Он — Повелитель Человечества и властелин мириадов людей, завоеванных могуществом Его неисчислимых функций.\n\n"
            "Он — полутруп, неуловимую искру жизни в котором поддерживают древние знания, и ради этого ежедневно приносится в жертву тысяча душ. И поэтому Владыка Человечества никогда не умирает по-настоящему.\n\n"
            "Даже в своем нынешнем состоянии Матан продолжает миссию, для которой появился на свет.\n\n"
            "Могучие боевые математики пересекают кишащий демонами обычный мир, и путь этот освещен Физтехом, зримым проявлением духовной воли Матана.\n\n"
            "Огромные армии сражаются во имя Его на бесчисленных полях тетрадей.\n\n"
            "Величайшие среди его солдат — преподы матана, математические десантники, генетически улучшенные супервоины.\n\n"
            "У них много товарищей по оружию: Аспирантская Гвардия и бесчисленные Силы Студенческой Обороны, вечно бдительный Деканат и техножрецы Адептус Информатикус.n\n"
            "Но, несмотря на все старания, их сил едва хватает, чтобы сдерживать извечную угрозу со стороны физиков, химиков и биологов.\n\n"
            "Быть физтехом в такое время — значит быть одним из тысяч.\n\n"
            "Это значит жить при самом жестоком и кровавом режиме, который только можно представить.\n\n"
            "Забудьте о достижениях общесоса и проги, ибо многое забыто и никогда не будет открыто заново.\n\n"
            "Забудьте о перспективах, обещанных прогрессом, о взаимопонимании, ибо во мраке МФТИ есть только война.\n\n"
            "Нет мира среди аудиторий — лишь вечная бойня и кровопролитие, да смех жаждущих фопфов.\n\n");
    fprintf(out,    "\\section{Дифференциация}\n\n");
    fprintf(out, "Функция количества еретиков от количества пар матана представляется как \n\n");
    return NOERR;
}

int TeXClose(FILE* out)
{
    fprintf (out, "\n\nВ результате мы видим, как ересь зависит от количества пар матана, что делать с этой информацией решаете вы сами\n\n");

    fprintf (out, "\\section{Список Литературы}\n\n");
    fprintf (out,   "1. https://github.com/aleksplast/Differentiator\n\n"
                    "2. Л.Н. Знаменская Дифференциальное и интегральное исчисления. Функции одной переменной\n\n"
                    "3. Кудрявцев Л.Д., Кутасов А.Д., Чехлов В.И., Шабунин М.И. Сборник задач по математическому анализу. Том 1. Предел. Непрерывность. Дифференцируемость Учеб пособие/ Под ред Л Д Кудрявцева — 2-е изд , перераб — М ФИЗМАТЛИТ, 2003 — 496 с —ISBN 5-9221-0306-7\n\n"
                    "4. Warhammer 40 000: Core Book (ENG) 60 04 01 99 142 Printed by CC in China\n\n"
            );

    fprintf (out, "\\end{document}");

    return NOERR;
}

int compare(const double a, const double b)
{
    DBG assert(isfinite(a));
    DBG assert(isfinite(b));

    if (fabs(a-b) < EPSILON)
        return 0;
    if ((a-b) > EPSILON)
        return 1;
    if ((a-b) < EPSILON)
        return -1;

    return NOERR;
}
