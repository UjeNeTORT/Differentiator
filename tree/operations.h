#ifndef OPERATIONS_H
#define OPERATIONS_H

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
    Opcodes opcode;
    const char * name;
    NodeType type; // binary or unary
};

const Operation OPERATIONS[] = // codestyle?
{
    {EQUAL, "=",      UN_OP},
    {ADD,   "+",      BI_OP},
    {SUB,   "-",      BI_OP},
    {MUL,   "*",      BI_OP},
    {DIV,   "/",      BI_OP},
    {POW,   "^",      BI_OP},
    {EXP,   "exp",    UN_OP},
    {SIN,   "sin",    UN_OP},
    {COS,   "cos",    UN_OP},
    {TG,    "tg",     UN_OP},
    {CTG,   "ctg",    UN_OP},
    {ASIN,  "arcsin", UN_OP},
    {ACOS,  "arccos", UN_OP},
    {ATG,   "arctg",  UN_OP},
    {ACTG,  "arcctg", UN_OP},
    {SH,    "sh",     UN_OP},
    {CH,    "ch",     UN_OP},
    {TH,    "th",     UN_OP},
    {CTH,   "cth",    UN_OP},
    {LOG,   "log",    BI_OP},
    {LN,    "ln",     UN_OP}
};

const int OPERATIONS_NUM = sizeof(OPERATIONS) / sizeof(Operation);

#endif // OPERATIONS_H
