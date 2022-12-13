#ifndef MIDDLEEND_H
#define MIDDLEEND_H

#include "..\functions\Config.h"
#include "middleend.h"
#include "..\functions\diftree.h"
#include "..\functions\text-sort.h"
#include "..\functions\common.h"

enum Side
{
    LEFT,
    RIGHT,
};

int SimplifyCode(Tree* tree);

int SimplifyConstants(Node* node);

int SimplifyConstantNode(Node* node);

int DeleteMeaninglessNodes(Node* node);

int CheckNDeleteNode(Node* node);

int ChangeIntoChild(Node* node, Side side);

int ChangeNodeIntoNum(Node* node, double val);

int DeleteAnchor(Node* node, Side side);

#endif //MIDDLEEND_H
