#ifndef RECURSIVE_DECENT_H
#define RECURSIVE_DECENT_H

#include "diftree.h"
#include "common.h"

Node** LexicAnalizer(char* str);

int StringAnalizer(char** s, Node*** nodes);

Node* GetGrammar(Node** nodes);

Node* GetFunc(Node*** arr);

Node* GetParams(Node*** arr);

Node* GetStatement(Node*** arr);

Node* GetWhile(Node*** arr);

Node* GetCall(Node*** arr);

Node* GetIf(Node*** arr);

Node* GetAssign(Node*** arr);

Node* GetRet(Node*** arr);

Node* GetExpression(Node*** arr);

Node* GetT(Node*** arr);

Node* GetPower(Node*** arr);

Node* GetP(Node*** arr);

Node* GetVar(Node*** arr);

Node* GetNumber(Node*** arr);

Node* CreateNode(NodeType type, double val, OperType optype, char* varvalue, Tree* tree, Node* ancestor, Node* leftchild, Node* rightchild);

#endif //RECURSIVE_DECENT
