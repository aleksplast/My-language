#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <strings.h>
#include "recursivedescent.h"

const int Cmdsize = 40;

#define CreateOpType(OpType) CreateNode(OP_TYPE, 0, OpType, NULL, NULL, NULL, NULL, NULL, 0)

const char* GetComArg(int argc, char* argv[])
{
    const char* input = "input.txt";

    if (argc > 1)
        return argv[1];
    else
        return input;
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

Node** LexicAnalizer(char* str, char*** namestable)
{
    char** s = &str;

    Node** nodes = (Node**) calloc(strlen(str) + 2, sizeof(Node*));

    Node** oldnodes = nodes;

    StringAnalizer(s, &nodes, namestable);

    return oldnodes;
}

int StringAnalizer(char** s, Node*** nodes, char*** namestable)
{
    double val = 0;
    int len = 0;
    char* name = NULL;
    char** oldtable = *namestable;
    int line = 1;

    while (**s != '\0')
    {
        char* cmd = (char*) calloc(Cmdsize, sizeof(char));
        while(isspace(**s))
        {
            if (**s == '\n')
                line++;
            (*s)++;
        }
        if (sscanf(*s, "%lg%n", &val, &len) == 1)
        {
            **nodes = CreateNode(NUM_TYPE, val, OP_UNKNOWN, NULL, NULL, NULL, NULL, NULL, line);
            (*s) += len;
            (*nodes)++;
            free(cmd);
        }
        else if (sscanf(*s, "%s%n", cmd, &len) == 1)
        {
            if (OperType optype = IsOper(cmd))
            {
                **nodes = CreateNode(OP_TYPE, 0, optype, NULL, NULL, NULL, NULL, NULL, line);
                free(cmd);
            }
            else
            {
                if ((name = CheckForNewName(cmd, namestable)) == NULL)
                    **nodes = CreateNode(VAR_TYPE, 0, OP_UNKNOWN, cmd, NULL, NULL, NULL, NULL, line);
                else
                {
                    **nodes = CreateNode(VAR_TYPE, 0, OP_UNKNOWN, name, NULL, NULL, NULL, NULL, line);
                    free(cmd);
                }
            }
            *s += len;
            (*nodes)++;
        }

        *namestable = oldtable;
    }

    **nodes = CreateNode(OP_TYPE, 0, OP_END, NULL, NULL, NULL, NULL, NULL, line);
    (*s)++;
    (*nodes)++;

    return NOERR;
}

char* CheckForNewName(char* cmd, char*** namestable)
{
    while (**namestable != NULL)
    {
        if (strcmp(cmd, **namestable) == 0)
            return **namestable;

        (*namestable)++;
    }

    **namestable = cmd;

    return NULL;
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
    fprintf(data, " { ");
    ContentPrint(data, node);

    if (node->leftchild)
        NodePrint(data, node->leftchild);

    if (node->rightchild)
        NodePrint(data, node->rightchild);

    fprintf(data, " } ");

    return NOERR;
}

int ContentPrint(FILE* data, Node* node)
{
    if (node->type == NUM_TYPE)
        fprintf(data, "%lg", node->val);

    else if (node->type == VAR_TYPE)
        fprintf(data, "%s", node->varvalue);

    else if (node->type == OP_TYPE)
        OpTypePrint(data, node->optype);

    return NOERR;
}

Node* GetGrammar(Node** nodes)
{
    Node* node = GetCode(&nodes);
    assert((*nodes)->optype == OP_END);

    return node;
}

Node* GetCode(Node*** arr)
{
    assert((**arr)->optype == OP_FUNC || (**arr)->optype == OP_VAR);

    Node* returnnode = CreateOpType(OP_STAT);
    Node* currnode = returnnode;
    Node* node = NULL;

    if ((**arr)->optype == OP_FUNC)
        node = GetFunc(arr);
    else
        node = GetAssign(arr);

    currnode->leftchild = node;

    while ((**arr)->optype == OP_FUNC || (**arr)->optype == OP_VAR)
    {
        Node* newnode = CreateOpType(OP_STAT);
        currnode->rightchild = newnode;
        currnode = newnode;
        node = NULL;

        if ((**arr)->optype == OP_FUNC)
            node = GetFunc(arr);
        else
            node = GetAssign(arr);

        currnode->leftchild = node;
    }

    return returnnode;
}

Node* GetFunc(Node*** arr)
{
    assert((**arr)->optype == OP_FUNC);

    Node* currnode = **arr;
    (*arr)++;

    Node* nodeL = **arr;
    (*arr)++;

    currnode->leftchild = nodeL;

    Node* nodeLL = GetParams(arr);
    Node* nodeR = GetStatement(arr);

    currnode->rightchild = nodeR;
    nodeL->leftchild = nodeLL;

    return currnode;
}

Node* GetParams(Node*** arr)
{
    assert((**arr)->optype == OP_OPBRC);
    (*arr)++;

    Node* paramnode = NULL;
    if ((**arr)->optype == OP_CLBRC)
    {
        (*arr)++;
        return paramnode;
    }

    paramnode = CreateOpType(OP_PARAM);
    Node* currnode = **arr;
    paramnode->leftchild = currnode;
    (*arr)++;

    Node* nodeL = **arr;
    (*arr)++;

    assert((**arr)->optype == OP_COMMA || (**arr)->optype == OP_CLBRC);
    (*arr)++;

    currnode->leftchild = nodeL;
    Node* returnnode = paramnode;

    while((**arr)->optype == OP_VAR)
    {
        Node* newnode = CreateOpType(OP_PARAM);
        currnode = **arr;
        newnode->leftchild = currnode;
        (*arr)++;

        nodeL = **arr;
        (*arr)++;

        assert((**arr)->optype == OP_COMMA || (**arr)->optype == OP_CLBRC);
        (*arr)++;

        currnode->leftchild = nodeL;
        paramnode->rightchild = newnode;
        paramnode = newnode;
    }

    return returnnode;
}

Node* GetStatement(Node*** arr)
{
    if ((**arr)->optype == OP_OPBLOCK || (**arr)->optype == OP_SEP)
        (*arr)++;

    Node* returnnode = NULL;
    Node* statnode = NULL;
    Node* oldstat = NULL;


    while ((**arr)->optype == OP_WHILE || (**arr)->optype == OP_IF || (**arr)->optype == OP_RET || (**arr)->type != OP_TYPE || (**arr)->optype == OP_VAR || (**arr)->optype == OP_IN || (**arr)->optype == OP_OUT)
    {
        statnode = CreateOpType(OP_STAT);
        if (returnnode == NULL)
            returnnode = statnode;

        Node* nodeL = NULL;

        if ((**arr)->optype == OP_WHILE)
            nodeL = GetWhile(arr);
        else if ((**arr)->optype == OP_IF)
            nodeL = GetIf(arr);
        else if ((**arr)->optype == OP_RET)
            nodeL = GetRet(arr);
        else if ((**arr)->optype == OP_VAR)
            nodeL = GetAssign(arr);
        else if ((**arr)->optype == OP_IN)
            nodeL = GetIn(arr);
        else if ((**arr)->optype == OP_OUT)
            nodeL = GetOut(arr);
        else
        {
            nodeL = GetExpression(arr);
            if ((**arr)->optype == OP_OPBRC)
            {
                Node* callnode = CreateOpType(OP_CALL);
                callnode->leftchild = nodeL;
                nodeL = callnode;

                Node* params = GetCall(arr);

                callnode->leftchild->leftchild = params;
            }
            else if ((**arr)->optype == OP_EQ)
            {
                Node* nodeLL = nodeL;

                nodeL = **arr;
                (*arr)++;

                Node* nodeRR = GetExpression(arr);

                if ((**arr)->optype == OP_OPBRC)
                {
                    Node* callnode = CreateOpType(OP_CALL);

                    nodeL->leftchild = nodeLL;
                    nodeL->rightchild = callnode;
                    callnode->leftchild = nodeRR;

                    Node* params = GetCall(arr);

                    nodeRR->leftchild = params;
                }
                else
                {
                    nodeL->leftchild = nodeLL;
                    nodeL->rightchild = nodeRR;
                }
            }
            assert((**arr)->optype == OP_SEP);
            (*arr)++;
        }

        statnode->leftchild = nodeL;
        if (oldstat != NULL)
            oldstat->rightchild = statnode;
        oldstat = statnode;
    }

    if ((**arr)->optype == OP_CLBLOCK)
        (*arr)++;

    return returnnode;
}

Node* GetIn(Node*** arr)
{
    assert((**arr)->optype == OP_IN);

    Node* returnnode = **arr;
    (*arr)++;

    assert((**arr)->optype == OP_OPBRC);
    (*arr)++;

    Node* nodeL = (**arr);
    returnnode->leftchild = nodeL;
    (*arr)++;

    assert((**arr)->optype == OP_CLBRC);
    (*arr)++;

    assert((**arr)->optype == OP_SEP);
    (*arr)++;

    return returnnode;
}

Node* GetOut(Node*** arr)
{
    assert((**arr)->optype == OP_OUT);

    Node* returnnode = **arr;
    (*arr)++;

    assert((**arr)->optype == OP_OPBRC);
    (*arr)++;

    Node* nodeL = (**arr);
    returnnode->leftchild = nodeL;
    (*arr)++;

    assert((**arr)->optype == OP_CLBRC);
    (*arr)++;

    assert((**arr)->optype == OP_SEP);
    (*arr)++;

    return returnnode;
}

Node* GetCall(Node*** arr)
{
    assert((**arr)->optype == OP_OPBRC);
    (*arr)++;

    Node* paramnode = NULL;
    if ((**arr)->type != VAR_TYPE && (**arr)->type != NUM_TYPE)
        return paramnode;

    paramnode = CreateOpType(OP_PARAM);
    Node* currnode = GetExpression(arr);

    paramnode->leftchild = currnode;

    assert((**arr)->optype == OP_COMMA || (**arr)->optype == OP_CLBRC);
    (*arr)++;

    Node* returnnode = paramnode;
    while((**arr)->type == VAR_TYPE || (**arr)->type == NUM_TYPE)
    {
        Node* newnode = CreateOpType(OP_PARAM);

        currnode = GetExpression(arr);
        newnode->leftchild = currnode;

        assert((**arr)->optype == OP_COMMA || (**arr)->optype == OP_CLBRC);
        (*arr)++;

        paramnode->rightchild = newnode;
        paramnode = newnode;
    }

    return returnnode;
}

Node* GetWhile(Node*** arr)
{
    assert((**arr)->optype == OP_WHILE);

    Node* returnnode = **arr;
    (*arr)++;

    assert((**arr)->optype == OP_OPBRC);
    (*arr)++;

    Node* nodeL = GetExpression(arr);

    assert((**arr)->optype == OP_CLBRC);
    (*arr)++;

    Node* nodeR = GetStatement(arr);

    returnnode->leftchild = nodeL;
    returnnode->rightchild = nodeR;

    return returnnode;
}

Node* GetIf(Node*** arr)
{
    assert((**arr)->optype == OP_IF);

    Node* returnnode = **arr;
    (*arr)++;

    assert((**arr)->optype == OP_OPBRC);
    (*arr)++;

    Node* nodeL = GetExpression(arr);

    assert((**arr)->optype == OP_CLBRC);
    (*arr)++;

    Node* nodeR = GetStatement(arr);
    if ((**arr)->optype == OP_ELSE)
    {
        Node* nodeelse = **arr;
        (*arr)++;

        Node* nodeRR = GetStatement(arr);

        returnnode->leftchild = nodeL;
        returnnode->rightchild = nodeelse;
        nodeelse->leftchild = nodeR;
        nodeelse->rightchild = nodeRR;
    }
    else
    {
        returnnode->leftchild = nodeL;
        returnnode->rightchild = nodeR;
    }

    return returnnode;
}

Node* GetAssign(Node*** arr)
{
    assert((**arr)->optype == OP_VAR);

    Node* returnnode = **arr;
    (*arr)++;

    Node* nodeL = GetExpression(arr);

    assert((**arr)->optype == OP_EQ);
    (*arr)++;

    Node* nodeR = GetExpression(arr);

    assert((**arr)->optype == OP_SEP);
    (*arr)++;

    returnnode->leftchild = nodeL;
    returnnode->rightchild = nodeR;

    return returnnode;
}

Node* GetRet(Node*** arr)
{
    assert((**arr)->optype == OP_RET);

    Node* returnnode = **arr;
    (*arr)++;

    Node* nodeL = GetExpression(arr);

    assert((**arr)->optype == OP_SEP);
    (*arr)++;

    returnnode->leftchild = nodeL;

    return returnnode;
}

Node* GetExpression(Node*** arr)
{
    Node* currnode = GetT(arr);
    Node* nodeL = NULL;

    while ((**arr)->optype == OP_ADD || (**arr)->optype == OP_SUB)
    {
        nodeL = currnode;
        currnode = **arr;
        (*arr)++;

        Node* nodeR = GetT(arr);
        currnode->leftchild = nodeL;
        currnode->rightchild = nodeR;
    }

    return currnode;
}

Node* GetT(Node*** arr)
{
    Node* currnode = GetPower(arr);
    Node* nodeL = NULL;

    while ((**arr)->optype == OP_MUL || (**arr)->optype == OP_DIV)
    {
        nodeL = currnode;
        currnode = **arr;
        (*arr)++;

        Node* nodeR = GetPower(arr);
        currnode->leftchild = nodeL;
        currnode->rightchild = nodeR;
    }

    return currnode;
}

Node* GetPower(Node*** arr)
{
    Node* currnode = GetP(arr);
    Node* nodeL = NULL;

    while((**arr)->optype == OP_POWER)
    {
        nodeL = currnode;
        currnode = **arr;
        (*arr)++;
        Node* nodeR = GetP(arr);
        currnode->leftchild = nodeL;
        currnode->rightchild = nodeR;
    }

    return currnode;
}

Node* GetP(Node*** arr)
{
    Node* node = **arr;
    if (node->optype == OP_OPBRC)
    {
        (*arr)++;
        node = GetExpression(arr);
        assert((**arr)->optype == OP_CLBRC);
        (*arr)++;
    }
    else if (node->type == VAR_TYPE)
        node = GetVar(arr);
    else if (node->type == NUM_TYPE)
        node = GetNumber(arr);

    return node;
}

Node* GetVar(Node*** arr)
{
    Node* node = **arr;

    (*arr)++;

    return node;
}

Node* GetNumber(Node*** arr)
{
    Node* node = **arr;

    (*arr)++;

    return node;
}

int CheckForError(Node* node, OperType optype)
{
    if (node->optype != optype)
    {
        ErrorPrint(node);
        exit(1);
    }

    return true;
}

int ErrorPrint(Node* node)
{
    if (node->optype == OP_SEP)
        printf("Missing separation token on line %d\n", node->line);
    else if (node->optype == OP_OPBRC)
        printf("Missing opening bracket on line %d\n", node->line);
    else if (node->optype == OP_CLBRC)
        printf("Missing closing bracket on line %d\n", node->line);
    else if (node->optype == OP_EQ)
        printf("Missing equality token on line %d\n", node->line);
    else if (node->optype == OP_VAR)
        printf("Missing variable token on line %d\n", node->line);
    else if (node->optype == OP_FUNC)
        printf("Missing function token on line %d\n", node->line);
    else if (node->optype == OP_COMMA)
        printf("Missing comma on line %d\n", node->line);

    return NOERR;
}
