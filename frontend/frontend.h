#ifndef FRONTEND_H
#define FRONTEND_H

#include "..\functions\diftree.h"
#include "..\functions\common.h"
#include "..\functions\TXLib.h"

const char* GetComArg(int argc, char* argv[]);

Node** LexicAnalizer(char* str, char*** namestable);

int StringAnalizer(char** s, Node*** nodes, char*** namestable);

char* CheckForNewName(char* cmd, char*** namestable);

int CheckForError(Node* node, OperType optype1, OperType optype2);

int ErrorPrint(Node* node, OperType optype);

void PrintDeclarationError(int line, char* cmd);

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

#endif //FRONTEND_H
