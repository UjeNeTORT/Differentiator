#ifndef OPERATIONS_H
#define OPERATIONS_H

const int ILL_OPNUM = 1000;

typedef enum
{
    EQUAL = 0,
    ADD  = 1,
    SUB  = 2,
    MUL  = 3,
    DIV  = 4,
    POW  = 5,
    EXP  = 6, // e^(x)
    SIN  = 7,
    COS  = 8,
    TG   = 9,
    CTG  = 10,
    ASIN = 11,
    ACOS = 12,
    ATG  = 13,
    ACTG = 14,
    SH   = 15,
    CH   = 16,
    TH   = 17,
    CTH  = 18,
    LOG  = 19,
    LN   = 20,
} Opcodes;

typedef enum
{
    ERR   = 0,
    NUM   = 1, // number
    VAR   = 2, // variable
    BI_OP = 3, // binary operator
    UN_OP = 4, // unary operator
} NodeType;

struct Operation
{
    Opcodes      opcode;
    const char * name;
    NodeType     type; // binary or unary
    int          priority;
};

const Operation OPERATIONS[] = // codestyle?
{
    {EQUAL, "=",      UN_OP, 0},
    {ADD,   "+",      BI_OP, 1},
    {SUB,   "-",      BI_OP, 1},
    {MUL,   "*",      BI_OP, 2},
    {DIV,   "/",      BI_OP, 2},
    {POW,   "^",      BI_OP, 3},
    {EXP,   "exp",    UN_OP, 4},
    {LN,    "ln",     UN_OP, 4},
    {SIN,   "sin",    UN_OP, 4},
    {COS,   "cos",    UN_OP, 4},
    {TG,    "tg",     UN_OP, 4},
    {CTG,   "ctg",    UN_OP, 4},
    {ASIN,  "arcsin", UN_OP, 4},
    {ACOS,  "arccos", UN_OP, 4},
    {ATG,   "arctg",  UN_OP, 4},
    {ACTG,  "arcctg", UN_OP, 4},
    {SH,    "sh",     UN_OP, 4},
    {CH,    "ch",     UN_OP, 4},
    {TH,    "th",     UN_OP, 4},
    {CTH,   "cth",    UN_OP, 4},
    {LOG,   "log",    BI_OP, 4},
};

const int OPERATIONS_NUM = sizeof(OPERATIONS) / sizeof(Operation);

#endif // OPERATIONS_H
