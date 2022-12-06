#include <stdio.h>
#include <assert.h>
#include <ctype.h>
#include <strings.h>
#include "recursivedescent.h"

const int Cmdsize = 40;

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

Node** LexicAnalizer(char* str)
{
    char** s = &str;

    Node** nodes = (Node**) calloc(strlen(str) + 2, sizeof(Node*));

    Node** oldnodes = nodes;

    StringAnalizer(s, &nodes);

    return oldnodes;
}

int StringAnalizer(char** s, Node*** nodes)
{
    double val = 0;
    int len = 0;

    while(isspace(**s))
        (*s)++;

    while (**s != '\0')
    {
        char* cmd = (char*) calloc(Cmdsize, sizeof(char));
        while(isspace(**s))
            (*s)++;
        if (sscanf(*s, "%lg%n", &val, &len) == 1)
        {
            **nodes = CreateNode(NUM_TYPE, val, OP_UNKNOWN, NULL, NULL, NULL, NULL, NULL);
            (*s) += len;
            (*nodes)++;
        }
        else if (sscanf(*s, "%s%n", cmd, &len) == 1)
        {
            if (OperType optype = IsOper(cmd))
                **nodes = CreateNode(OP_TYPE, 0, optype, NULL, NULL, NULL, NULL, NULL);
            else
                **nodes = CreateNode(VAR_TYPE, 0, OP_UNKNOWN, cmd, NULL, NULL, NULL, NULL);
            *s += len;
            (*nodes)++;
        }
    }

    **nodes = CreateNode(OP_TYPE, 0, OP_END, NULL, NULL, NULL, NULL, NULL);
    (*s)++;
    (*nodes)++;

    return NOERR;
}

Node* GetGrammar(Node** nodes)
{
    Node* node = GetFunc(&nodes);
    assert((*nodes)->optype == OP_END);

    return node;
}

Node* GetFunc(Node*** arr)
{
    assert((**arr)->optype == OP_FUNC);

    Node* funcnode = CreateNode(OP_TYPE, 0, OP_STAT, NULL, NULL, NULL, NULL, NULL);
    Node* returnnode = funcnode;

    Node* currnode = **arr;
    funcnode->leftchild = currnode;
    (*arr)++;
    Node* nodeL = **arr;
    (*arr)++;
    currnode->leftchild = nodeL;
    Node* nodeLL = GetParams(arr);
    Node* nodeR = GetStatement(arr);
    currnode->rightchild = nodeR;
    nodeL->leftchild = nodeLL;

    while((**arr)->optype == OP_FUNC)
    {
        Node* newnode = CreateNode(OP_TYPE, 0, OP_STAT, NULL, NULL, NULL, NULL, NULL);
        currnode = **arr;
        newnode->leftchild = currnode;
        (*arr)++;
        nodeL = **arr;
        (*arr)++;
        currnode->leftchild = nodeL;
        nodeLL = GetParams(arr);
        nodeL->leftchild = nodeLL;
        Node* nodeR = GetStatement(arr);
        currnode->rightchild = nodeR;
        funcnode->rightchild = newnode;
        funcnode = newnode;
    }

    return returnnode;
}

Node* GetParams(Node*** arr)
{
    assert((**arr)->optype == OP_OPBRC);
    (*arr)++;

    Node* paramnode = NULL;
    if ((**arr)->optype != OP_VAR)
        return paramnode;

    paramnode = CreateNode(OP_TYPE, 0, OP_PARAM, NULL, NULL, NULL, NULL, NULL);
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
        Node* newnode = CreateNode(OP_TYPE, 0, OP_PARAM, NULL, NULL, NULL, NULL, NULL);
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


    while ((**arr)->optype == OP_WHILE || (**arr)->optype == OP_IF || (**arr)->optype == OP_RET || (**arr)->type != OP_TYPE || (**arr)->optype == OP_VAR)
    {
        statnode = CreateNode(OP_TYPE, 0, OP_STAT, NULL, NULL, NULL, NULL, NULL);
        if (returnnode == NULL)
            returnnode = statnode;

        Node* nodeL = NULL;

        if ((**arr)->optype == OP_WHILE)
            nodeL = GetWhile(arr);
        else if ((**arr)->optype == OP_IF)
        {
            nodeL = GetIf(arr);
        }
        else if ((**arr)->optype == OP_RET)
            nodeL = GetRet(arr);
        else if ((**arr)->optype == OP_VAR)
            nodeL = GetAssign(arr);
        else
        {
            nodeL = GetExpression(arr);
            if ((**arr)->optype == OP_OPBRC)
            {
                Node* callnode = CreateNode(OP_TYPE, 0, OP_CALL, NULL, NULL, NULL, NULL, NULL);
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
                    Node* callnode = CreateNode(OP_TYPE, 0, OP_CALL, NULL, NULL, NULL, NULL, NULL);

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

Node* GetCall(Node*** arr)
{
    assert((**arr)->optype == OP_OPBRC);
    (*arr)++;

    Node* paramnode = NULL;
    if ((**arr)->type != VAR_TYPE && (**arr)->type != NUM_TYPE)
        return paramnode;

    paramnode = CreateNode(OP_TYPE, 0, OP_PARAM, NULL, NULL, NULL, NULL, NULL);
    Node* currnode = GetExpression(arr);

    paramnode->leftchild = currnode;

    assert((**arr)->optype == OP_COMMA || (**arr)->optype == OP_CLBRC);
    (*arr)++;

    Node* returnnode = paramnode;
    while((**arr)->type == VAR_TYPE || (**arr)->type == NUM_TYPE)
    {
        Node* newnode = CreateNode(OP_TYPE, 0, OP_PARAM, NULL, NULL, NULL, NULL, NULL);

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
