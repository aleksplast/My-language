#ifndef TREE_H
#define TREE_H

#define DEBUG

#ifndef DEBUG
#define DBG if(false)
#else
#define DBG if(true)
#endif

typedef char* elem_t;

enum NodeType
{
    UNKNOWN_TYPE    = 0,
    OP_TYPE         = 1,
    NUM_TYPE,
    VAR_TYPE,
};

enum OperType
{
    OP_UNKNOWN = 0,
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_OPBLOCK,
    OP_CLBLOCK,
    OP_POWER,
    OP_AND,
    OP_OR,
    OP_IF,
    OP_ELSE,
    OP_EQ,
    OP_VAR,
    OP_WHILE,
    OP_FUNC,
    OP_SEP,
    OP_RET,
    OP_OPBRC,
    OP_CLBRC,
    OP_STAT,
    OP_COMMA,
    OP_PARAM,
    OP_CALL,
    OP_END,
};

struct Tree;

struct Node
{
    NodeType type;
    double val;
    OperType optype;
    char* varvalue;

    Node* ancestor;
    Node* leftchild;
    Node* rightchild;

    Tree* tree;
};

struct Tree
{
    Node* anchor;
    int size;

    char* graphlog;
};

int TreeCtor(Tree* tree, NodeType type, double val, OperType optype, char* varvalue, char* graphfile);

Node* NodeCtor(NodeType type, double val, OperType optype, char* varvalue);

int TreeVerr(Tree* tree);

int NodeVerr (Node* node, int* errors);

int TreeGraphDump (Tree* tree, int errors, int line, const char* func, const char* file);

int NodeDump(Node* node, int* counter, FILE* pic);

Node* AddRightChild(Tree* tree, Node* ancestor);

Node* AddLeftChild(Tree* tree, Node* ancestor);

int TypePrint(FILE* fp, NodeType type);

int OpTypePrint(FILE* fp, OperType opertype);

int NodeDetor(Node* node);

Node* TreeDepthSearch(Node* node, char* searchvar);

int TreeDetor(Tree* tree);

OperType IsOper(char* str);

int CreateAncestor(Node* node, Node* ancestor, Tree* tree);

enum TreeErrors
{
    ANCHORERR       = 1 << 1,
    VALERR          = 1 << 2,
    SIZEERR         = 1 << 3,
    NODETREEERR     = 1 << 4,
};

#endif //TREE_H