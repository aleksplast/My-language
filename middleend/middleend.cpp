#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <strings.h>
#include <ctype.h>

#include "middleend.h"

static bool CHANGE = true;

#define IS_OP(oper) (node->type == OP_TYPE) && (node->optype == oper)
#define IS_LVAL(value) (node->leftchild->type == NUM_TYPE) && (compare(node->leftchild->val, value) == 0)
#define IS_RVAL(value) (node->rightchild->type == NUM_TYPE) && (compare(node->rightchild->val, value) == 0)
#define ArithOper(oper) node->val = node->leftchild->val oper node->rightchild->val

int SimplifyCode(Tree* tree)
{
    DBG assert(tree != NULL);

    CHANGE = true;

    while (CHANGE)
    {
        SimplifyConstants(tree->anchor);

        DeleteMeaninglessNodes(tree->anchor);
    }

    return NOERR;
}

int SimplifyConstants(Node* node)
{
    DBG assert(node != NULL);

    CHANGE = false;

    if (node->leftchild)
        SimplifyConstants(node->leftchild);

    if (node->rightchild)
        SimplifyConstants(node->rightchild);

    SimplifyConstantNode(node);

    return NOERR;
}

int SimplifyConstantNode(Node* node)
{
    DBG assert(node != NULL);

    if (node->type != OP_TYPE)
        return NOERR;

    if (!node->leftchild)
        return NOERR;

    if (!node->rightchild)
        return NOERR;

    if (node->leftchild->type != NUM_TYPE || node->rightchild->type != NUM_TYPE)
        return NOERR;

    if (node->optype > OP_DIV)
        return NOERR;

    if (node->optype == OP_DIV)
        ArithOper(/);

    else if (IS_OP(OP_SUB))
        ArithOper(-);

    else if (IS_OP(OP_MUL))
        ArithOper(*);

    else if (IS_OP(OP_ADD))
        ArithOper(+);

    node->type = NUM_TYPE;
    node->varvalue = NULL;
    node->optype = OP_UNKNOWN;
    node->leftchild = NULL;
    node->rightchild = NULL;

    CHANGE = true;

    return NOERR;
}

int DeleteMeaninglessNodes(Node* node)
{
    DBG assert(node != NULL);

    if (node->leftchild)
        DeleteMeaninglessNodes(node->leftchild);
    if (node->rightchild)
        DeleteMeaninglessNodes(node->rightchild);

    CheckNDeleteNode(node);

    return NOERR;
}

int CheckNDeleteNode(Node* node)
{
    DBG assert (node != NULL);

    if (node->type != OP_TYPE)
        return NOERR;
    if (!node->leftchild || !node->rightchild)
        return NOERR;
    if (node->optype > OP_DIV)
        return NOERR;

    if (node->rightchild->type == NUM_TYPE && (node->optype == OP_ADD || node->optype == OP_MUL))
    {
        Node* temp = node->rightchild;
        node->rightchild = node->leftchild;
        node->leftchild = temp;
    }

    if (IS_OP(OP_MUL) && IS_LVAL(1))
        ChangeIntoChild(node, RIGHT);

    else if (IS_OP(OP_MUL) && IS_LVAL(0))
        ChangeNodeIntoNum(node, 0);

    else if (IS_OP(OP_ADD) && IS_LVAL(0))
        ChangeIntoChild(node, RIGHT);

    else if (IS_OP(OP_SUB) && IS_LVAL(0))
        ChangeIntoChild(node, RIGHT);

    else if (IS_OP(OP_POWER) && IS_RVAL(0))
        ChangeNodeIntoNum(node, 1);

    else if (IS_OP(OP_POWER) && IS_RVAL(1))
        ChangeIntoChild(node, LEFT);

    else if (IS_OP(OP_DIV) && IS_LVAL(0))
        ChangeNodeIntoNum(node, 0);

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
        NodeDetor(killchild);
        free(node);
    }
    else if (node == node->ancestor->rightchild)
    {
        node->ancestor->rightchild = child;
        child->ancestor = node->ancestor;
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
