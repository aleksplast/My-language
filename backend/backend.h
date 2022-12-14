#ifndef BACKEND_H
#define BACKEND_H

#include "..\functions\Config.h"
#include "..\functions\text-sort.h"
#include "..\functions\common.h"
#include "..\functions\diftree.h"
//#include "..\frontend\frontend.h"
#include "..\functions\stack.h"

const int STRSIZE = 50;

typedef poem Source;

int LanguageMain(const char* input);

char* TranslitIntoEnglish(char* str, long size);

Source InputHandler(const char* input);

int CreateAsmProgramm(Node* node, FILE* out);

int PrepareProg(Node* node, FILE* out, stack* nametablestk);

int AddCoreFunctions(FILE* out);

int NodeIntoAsmCode(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeStatement(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeIf(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeElse(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeWhile(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeFunc(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeNum(Node* node, FILE* out);

int AsmCodeArithOper(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeVar(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeEq(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeParam(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeRet(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeVariable(Node* node, stack* nametablestk, FILE* out);

int AsmCodeCall(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeIn(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AsmCodeOut(Node* node, int* labelcounter, stack* nametablestk, FILE* out);

int AddToNamesTable(stack* nametablestk, char* name);

int PushFromNamesTable(FILE* out, stack* nametablestk, char* name);

int PopFromNamesTable(FILE* out, stack* nametablestk, char* name);

int PopBackwardsFromNamesTable(FILE* out, stack* nametablestk, char* name);

int FillNamesTable(Node* node, stack* namestablestk);

int JumpPrint(FILE* out, OperType optype, int label);

NamesTable CreateNameTable();

int ReduceRax(FILE* out, stack* nametablestk);

int EnlargeRax(FILE* out, stack* nametablestk);

enum RussianSymb
{
    ARUS = -32,
    BRUS = -31,
    VRUS = -30,
    GRUS = -29,
    DRUS = -28,
    ERUS = -27,
    ZHRUS = -26,
    ZRUS = -25,
    IRUS = -24,
    YIRUS = -23,
    KRUS = -22,
    LRUS = -21,
    MRUS = -20,
    NRUS = -19,
    ORUS = -18,
    PRUS = -17,
    RRUS = -16,
    SRUS = -15,
    TRUS = -14,
    URUS = -13,
    FRUS = -12,
    HRUS = -11,
    TSRUS = -10,
    CHRUS = -9,
    SHRUS = -8,
    SHCHRUS = -7,
    IERUS = -6,
    YRUS = -5,
    QRUS = -4,
    EERUS = -3,
    IURUS = -2,
    IARUS = -1,
    ABRUS = -64,
    BBRUS = -63,
    VBRUS = -62,
    GBRUS = -61,
    DBRUS = -60,
    EBRUS = -59,
    ZHBRUS = -58,
    ZBRUS = -57,
    IBRUS = -56,
    YIBRUS = -55,
    KBRUS = -54,
    LBRUS = -53,
    MBRUS = -52,
    NBRUS = -51,
    OBRUS = -50,
    PBRUS = -49,
    RBRUS = -48,
    SBRUS = -47,
    TBRUS = -46,
    UBRUS = -45,
    FBRUS = -44,
    HBRUS = -43,
    TSBRUS = -42,
    CHBRUS = -41,
    SHBRUS = -40,
    SHCHBRUS = -39,
    IEBRUS = -38,
    YBRUS = -37,
    QBRUS = -36,
    EEBRUS = -35,
    IUBRUS = -34,
    IABRUS = -33,
};

#endif //BACKEND_H
