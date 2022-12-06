#define _ ,
#define POP StackPop(&cpu->commands)
#define PUSH(x) StackPush(&cpu->commands, x)

#define JUMP(sign, numb)                                                    \
        elem_t val1 = POP;                                                  \
        elem_t val2 = POP;                                                  \
        if (compare(val1, val2) sign numb)                                  \
            SetLabel(cpu);                                                  \
        else                                                                \
            cpu->ip += sizeof(int);                                         \
        CPUCHECK

#define ARITHOPER(oper, type, ...)                                          \
        type val1 = (type) POP;                                             \
        type val2 = (type) POP;                                             \
        __VA_ARGS__                                                         \
        PUSH(val1 oper val2);                                               \
        CPUCHECK


DEF_CMD(PUSH, 1, 1,
    {
        GetArg(cpu, cmd);
        PUSH(arg);
        CPUCHECK
    })

DEF_CMD(ADD, 2 , 0,
    {
        ARITHOPER(+, elem_t)
    })

DEF_CMD(SUB, 3, 0,
    {
        ARITHOPER(-, elem_t)
    })

DEF_CMD(MUL, 4, 0,
    {
        ARITHOPER(*, elem_t)
    })

DEF_CMD(DIV, 5, 0,
    {
        ARITHOPER(/, elem_t,
        if (compare(val2, 0) == 0)
        {
            printf("Error: cannot divide by 0");
            return ARITHERR;
        })
    })

DEF_CMD(OUT, 6, 0,
    {
        printf("YOUR VALUE = %lg\n", POP);
        CPUCHECK
    })

DEF_CMD(HLT, 7, 0,
    {
        CPUCHECK
        return NOERR;
    })

DEF_CMD(IN, 8, 0,
    {
        elem_t val = 0;
        scanf("%lg", &val);
        PUSH(val);
        CPUCHECK
    })

DEF_CMD(POP, 9, 1,
    {
        GetArg(cpu, cmd);
        *argptr = POP;
        CPUCHECK
    })

DEF_CMD(JMP, 10, 1,
    {
        SetLabel(cpu);
        CPUCHECK
    })

DEF_CMD(JB, 11, 1,
    {
        JUMP(<=, -1)
    })

DEF_CMD(JBE, 12, 1,
    {
        JUMP(<=, 0)
    })

DEF_CMD(JA, 13, 1,
    {
        JUMP(>=, 1)
    })

DEF_CMD(JAE, 14, 1,
    {
        JUMP(>=, 0)
    })

DEF_CMD(JE, 15, 1,
    {
        JUMP(==, 0)
    })

DEF_CMD(JNE, 16, 1,
    {
        JUMP(!=, 0)
    })

DEF_CMD(CALL, 17, 1,
    {
        StackPush(&cpu->returns, (elem_t) (cpu->ip + sizeof(int)));
        SetLabel(cpu);
        CPUCHECK
    })

DEF_CMD(SQRT, 18, 0,
    {
        elem_t a = POP;
        if (compare(a, 0) == -1)
        {
            printf("Error: cannot take square roots of negative numbers");
            return ARITHERR;
        }
        PUSH(sqrt(a));
        CPUCHECK
    })

DEF_CMD(RET, 19, 0,
    {
        cpu->ip = (int) StackPop(&cpu->returns);
        CPUCHECK
    })

DEF_CMD(SQRD, 20, 0,
    {
        elem_t a = POP;
        PUSH(a * a);
        CPUCHECK
    })

DEF_CMD(MOD, 21, 0,
    {
        ARITHOPER(%, int)
    })

DEF_CMD(IDIV, 22, 0,
    {
        ARITHOPER(/, int)
    })

DEF_CMD(RAMW, 23, 0,
    {
        RamWrite(cpu);
        CPUCHECK
    })

DEF_CMD(FLOOR, 24, 0,
    {
        PUSH((int) POP);
    })
