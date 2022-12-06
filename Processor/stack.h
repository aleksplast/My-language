#ifndef STACK_H
#define STACK_H

#include "Config.h"

#define DEBUG

#ifndef DEBUG
#define DBG if(false)
#else
#define DBG if(true)
#endif

struct stack{
    unsigned long long canaryleft;
    unsigned long long* dataguardl;
    elem_t* data;
    unsigned long long* dataguardr;
    size_t size;
    size_t capacity;
    unsigned int datahash;
    unsigned int structhash;
    unsigned long long canaryright;
};

enum StackErrors{
    STKPTRERR   = 1,
    DATAERR     = 2,
    SIZERR      = 4,
    CAPERR      = 8,
    SIZENCAPERR = 16,
    CANERR      = 32,
    HASHERR     = 64
};

#ifndef SYSERRORS
#define SYSERRORS
enum Errors
{
    NOERR = 0,      ///<No errors occured
    INPUTERR,     ///<Error with input file
    MEMERR,       ///<Error with memory
    FILERR       ///<Error with file
};

#endif //SYSERRORS

//!--------------------------------
//! @brief Constructs a stack with given capacity
//! @param[in] capacity Capacity of a constructing stack
//! @param[out] stk Stack to construct
//! @return Number of error, if it happened
//!--------------------------------
int StackCtor(struct stack* stk, size_t capacity);

//!--------------------------------
//! @brief Fills with poison free space of the stack
//! @param[in] left Start of a free space in stack
//! @param[in] right End of a free space in stack
//! @param[out] stk Pointer to the structure, in which strings contains
//! @return Number of error, if it happened
//!--------------------------------
int FillWPoison(struct stack* stk, int left, int right);

//!--------------------------------
//! @brief Takes last element of the stack
//! @param[out] stk Stack to pop from
//! @return Last element of the stack
//!--------------------------------
elem_t StackPop(struct stack* stk);

//!--------------------------------
//! @brief Pushes new value into stack
//! @param[in] elem Element to push
//! @param[out] stk Stack for pushing
//! @return Number of error, if it happened
//!--------------------------------
int StackPush(struct stack* stk, elem_t elem);

//!--------------------------------
//! @brief Enlarges capacity of the stack
//! @param[out] stk Stack to enlarge
//! @return Number of error, if it happened
//!--------------------------------
int StackRealloc(struct stack* stk);

//!--------------------------------
//! @brief Shrinks the size of the stack
//! @param[out] stk Stack to shrink
//! @return Number of error, if it happened
//!--------------------------------
int StackShrink(struct stack* stk);

//!--------------------------------
//! @brief Counts errors of the stack
//! @param[out] stk Stack for error counting
//! @return Number of errors
//!--------------------------------
int StackErr(struct stack* stk);

//!--------------------------------
//! @brief Writes full info of the stack into log files
//! @param[in] errors Number of errors of the stack
//! @param[in] line Line, where the dump happened
//! @param[in] func Function, where the dump happened
//! @param[in] file File, where the dump happened
//! @param[out] stk Pointer to the structure, in which strings contains
//! @return Number of error, if it happened
//!--------------------------------
int StackDump(struct stack* stk, int errors, int line, const char* func, const char* file);

//!--------------------------------
//! @brief Deconstructs the stac
//! @param[out] stk Stack to deconstruct
//! @return Number of error, if it happened
//!--------------------------------
int StackDetor(struct stack* stk);

//!--------------------------------
//! @brief Counts hash of a data
//! @param[in] lenght Size of data in bytes
//! @param[in] seed Seed for the hash
//! @param[out] data Data for the hash counting
//! @return Value of a hash
//!--------------------------------
unsigned int MurMurHash(const void* data, int lenght, unsigned int seed);

//!--------------------------------
//! @brief Updates hash for the stack
//! @param[out] stk Stack to update
//!--------------------------------
void UpdateHash(struct stack* stk);

int print(FILE* fp, long x);

int print(FILE* fp, char* x);

int print(FILE* fp, char x);

int print(FILE* fp, double x);

int print(FILE* fp, int x);

double GetPoison(double x);

int GetPoison(int x);

char GetPoison(char x);

long GetPoison(long x);

char* GetPoison(char* x);

#endif //STACK_H
