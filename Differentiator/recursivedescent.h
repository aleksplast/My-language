#ifndef RECURSIVE_DECENT_H
#define RECURSIVE_DECENT_H

#include "diftree.h"
#include "common.h"

const char* GetComArg(int argc, char* argv[]);

Node** LexicAnalizer(char* str, char*** namestable);

int StringAnalizer(char** s, Node*** nodes, char*** namestable);

char* CheckForNewName(char* cmd, char*** namestable);

int DataPrint(Node* node);

int NodePrint(FILE* data, Node* node);

int ContentPrint(FILE* data, Node* node);

Node* GetGrammar(Node** nodes);

Node* GetCode(Node*** arr);

Node* GetFunc(Node*** arr);

Node* GetParams(Node*** arr);

Node* GetStatement(Node*** arr);

Node* GetWhile(Node*** arr);

Node* GetCall(Node*** arr);

Node* GetIf(Node*** arr);

Node* GetAssign(Node*** arr);

Node* GetRet(Node*** arr);

Node* GetIn(Node*** arr);

Node* GetOut(Node*** arr);

Node* GetExpression(Node*** arr);

Node* GetT(Node*** arr);

Node* GetPower(Node*** arr);

Node* GetP(Node*** arr);

Node* GetVar(Node*** arr);

Node* GetNumber(Node*** arr);

Node* CreateNode(NodeType type, double val, OperType optype, char* varvalue, Tree* tree, Node* ancestor, Node* leftchild, Node* rightchild, int line);

#endif //RECURSIVE_DECENT
