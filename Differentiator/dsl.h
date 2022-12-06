#define DL DiffNode(node->leftchild, phrases, var, out)
#define DR DiffNode(node->rightchild, phrases, var, out)
#define CL NodeCopy(node->leftchild)
#define CR NodeCopy(node->rightchild)
#define SUB(L, R) CreateNode(OP_TYPE, 0, OP_SUB, NULL, node->tree, node, L, R)
#define MUL(L, R) CreateNode(OP_TYPE, 0, OP_MUL, NULL, node->tree, node, L, R)
#define DIV(L, R) CreateNode(OP_TYPE, 0, OP_DIV, NULL, node->tree, node, L, R)
#define ADD(L, R) CreateNode(OP_TYPE, 0, OP_ADD, NULL, node->tree, node, L, R)
#define SIN(L, R) CreateNode(OP_TYPE, 0, OP_SIN, NULL, node->tree, node, L, R)
#define COS(L, R) CreateNode(OP_TYPE, 0, OP_COS, NULL, node->tree, node, L, R)
#define SQRT(L, R) CreateNode(OP_TYPE, 0, OP_SQRT, NULL, node->tree, node, L, R)
#define POWER(L, R) CreateNode(OP_TYPE, 0, OP_POWER, NULL, node->tree, node, L, R)
#define LN(L,R) CreateNode(OP_TYPE, 0, OP_LN, NULL, node->tree, node, L, R)
#define CH(L,R) CreateNode(OP_TYPE, 0, OP_CH, NULL, node->tree, node, L, R)
#define SH(L, R) CreateNode(OP_TYPE, 0, OP_SH, NULL, node->tree, node, L, R)
#define IS_OP(oper) (node->type == OP_TYPE) && (node->optype == oper)
#define IS_LVAL(value) (node->leftchild->type == NUM_TYPE) && (compare(node->leftchild->val, value) == 0)
#define IS_RVAL(value) (node->rightchild->type == NUM_TYPE) && (compare(node->rightchild->val, value) == 0)
#define CREATENUM(num) CreateNode(NUM_TYPE, num, OP_UNKNOWN, NULL, node->tree, node, NULL, NULL)
#define CREATEVAR(var) CreateNode(VAR_TYPE, 0, OP_UNKNOWN, var, node->tree, node, NULL, NULL)
#define NEWNUM(num) CreateNode(NUM_TYPE, num, OP_UNKNOWN, NULL, NULL, node, NULL, NULL)
#define NEWVAR(var) CreateNode(VAR_TYPE, 0, OP_UNKNOWN, var, NULL, node, NULL, NULL)