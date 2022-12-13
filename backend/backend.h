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
    ARUS = -80,
    BRUS = -79,
    VRUS = -78,
    GRUS = -77,
    DRUS = -76,
    ERUS = -75,
    ZHRUS = -74,
    ZRUS = -73,
    IRUS = -72,
    YIRUS = -71,
    KRUS = -70,
    LRUS = -69,
    MRUS = -68,
    NRUS = -67,
    ORUS = -66,
    PRUS = -65,
    RRUS = -128,
    SRUS = -127,
    TRUS = -126,
    URUS = -125,
    FRUS = -124,
    HRUS = -123,
    TSRUS = -122,
    CHRUS = -121,
    SHRUS = -120,
    SHCHRUS = -119,
    IERUS = -118,
    YRUS = -117,
    QRUS = -116,
    EERUS = -115,
    IURUS = -114,
    IARUS = -113,
    ABRUS = -112,
    BBRUS = -111,
    VBRUS = -110,
    GBRUS = -109,
    DBRUS = -108,
    EBRUS = -107,
    ZHBRUS = -106,
    ZBRUS = -105,
    IBRUS = -104,
    YIBRUS = -103,
    KBRUS = -102,
    LBRUS = -101,
    MBRUS = -100,
    NBRUS = -99,
    OBRUS = -98,
    PBRUS = -97,
    RBRUS = -96,
    SBRUS = -95,
    TBRUS = -94,
    UBRUS = -93,
    FBRUS = -92,
    HBRUS = -91,
    TSBRUS = -90,
    CHBRUS = -89,
    SHBRUS = -88,
    SHCHBRUS = -87,
    IEBRUS = -86,
    YBRUS = -85,
    QBRUS = -84,
    EEBRUS = -83,
    IUBRUS = -82,
    IABRUS = -81,
    USELESS1 = -47,
    USELESS2 = -48,
};

#endif //BACKEND_H
