#ifndef TRANSLATOR_H
#define TRANSLATOR_H


#include "..\functions\Config.h"
#include "..\functions\common.h"
#include "..\functions\text-sort.h"
#include "..\functions\diftree.h"
#include "..\functions\TXLib.h"

int TranslateStandart(Node* node, FILE* out);

int PrintStart(Node* node, FILE* out);

int PrintEnd(Node* node, FILE* out);

int PrintMiddle(Node* node, FILE* out);

int Priority(Node* node);

#endif //TRANSLATOR_H
