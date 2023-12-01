/*************************************************************************
 * (c) 2023 Tikhonov Yaroslav (aka UjeNeTORT)
 *
 * email: tikhonovty@gmail.com
 * telegram: https://t.me/netortofficial
 * GitHub:   https://github.com/UjeNeTORT
 * repo:     https://github.com/UjeNeTORT/Differentiator
 *************************************************************************/

#ifndef TREE_H
#define TREE_H

#include <stdio.h>

#include "operations.h"
#include "../mylib/mylib.h"

const int MAX_TREE = 5000;
const int MAX_OP   = 10; // max len of operator or variable name // todo rename

const int NAMETABLE_CAPACITY = 10;

const double EPS = 1e-7;

const double EXPONENT = 2.718281828;
const double PI       = 3.141592654;

typedef enum
{
    TREE_DTOR_SUCCESS    = 0,
    TREE_DTOR_ERR_PARAMS = 1,
} TreeDtorRes;

typedef enum
{
    NAMETABLE_COPY_SUCCESS    = 0,
    NAMETABLE_COPY_ERR_PARAMS = 1,
} NameTableCopyRes;

typedef enum
{
    TRVRS_TREE_SUCCESS    = 0,
    TRVRS_TREE_ERR        = 1,
    TRVRS_TREE_ERR_PARAMS = 2,
} TraverseTreeRes;

typedef enum
{
    TREE_NODE_DTOR_SUCCESS = 0,
} TreeNodeDtorRes;

typedef enum
{
    PREORDER  = -1,
    INORDER   = 0,
    POSTORDER = 1,
} TraverseOrder;

typedef enum
{
    LEFT    = -1,
    REPLACE = 0,
    RIGHT   = 1,
} NodeLocation;

typedef enum
{
    WRT_NOD_DATA_SUCCESS = 0,
    WRT_NOD_DATA_ERR     = 1,
} WriteNodeDataRes;

typedef enum
{
    READ_ASSIGN_VAR_SUCCESS    = 0,
    READ_ASSIGN_VAR_ERR        = 1,
    READ_ASSIGN_VAR_ERR_PARAMS = 2,
} ReadAssignVariableRes;

typedef enum
{
    READ_ASSIGN_OP_SUCCESS       = 0,
    READ_ASSIGN_OP_ERR_NOT_FOUND = 1,
    READ_ASSIGN_OP_ERR_PARAMS    = 2,
} ReadAssignOperatorRes;

typedef enum
{
    READ_ASSIGN_DBL_SUCCESS     = 0,
    READ_ASSIGN_DBL_ERR         = 1,
    READ_ASSIGN_DBL_ERR_PARAMS  = 2,
} ReadAssignDoubleRes;

struct NameTable
{
    char * names [NAMETABLE_CAPACITY];
    double vals  [NAMETABLE_CAPACITY];
    int    free;
};

struct NodeData
{
    NodeType type;
    double   val;
};

struct TreeNode
{
    NodeData data;

    TreeNode * left;
    TreeNode * right;
};

struct Tree
{
    TreeNode * root;
    NameTable nametable;

    int size;
};

typedef int (* NodeAction_t) (TreeNode * node);

double Eval (const TreeNode * node, const NameTable * nametable);

TreeNode* TreeNodeCtor (double val, NodeType type, TreeNode * left, TreeNode * right);
int       TreeNodeDtor (TreeNode * node);

Tree*       TreeCtor   ();
TreeDtorRes TreeDtor   (Tree * tree);

Tree*            TreeCopy      (const Tree * tree);
TreeNode*        SubtreeCopy   (TreeNode * node);
NameTableCopyRes NameTableCopy (NameTable * dst, const NameTable * src);

TraverseTreeRes  TraverseTree     (Tree * tree, NodeAction_t NodeAction, TraverseOrder traverse_order);
TraverseTreeRes  TraverseTreeFrom (Tree * tree, TreeNode * node, NodeAction_t NodeAction, TraverseOrder traverse_order);

TreeNode*  SubtreeFind (TreeNode * node, double val, NodeType type);
TreeNode*  TreeFind    (Tree * tree, double val, NodeType type);

Tree*      ReadTree     (FILE * stream);
Tree*      ReadTree     (const char * infix_tree);
TreeNode*  ReadSubtree  (const char * infix_tree, NameTable * nametable, int * offset);
NodeData   ReadNodeData (const char * infix_tree, NameTable * nametable, int * offset);

int WriteSubtree  (FILE * stream, const TreeNode * node, const NameTable * nametable);
int WriteTree     (FILE * stream, const Tree * tree);
WriteNodeDataRes WriteNodeData (FILE * stream, NodeData data, const NameTable * nametable);

int UpdNameTable     (NameTable * nametable, char * word);
int IncorrectVarName (const char * word);

ReadAssignVariableRes ReadAssignVariable (NodeData * data, char * word, NameTable * nametable);
ReadAssignOperatorRes ReadAssignOperator (NodeData * data, char * word);
ReadAssignDoubleRes   ReadAssignDouble   (NodeData * data, char * word);

int ScanVariableVal (NameTable * nametable, int var_id);

int FindOperation (int opcode);

int FindNametableDups(NameTable * nametable, const char * word);

int IsDouble (char * word); // ! WARNING cructh function

int IsZero (double num);

int PrintfDebug   (const char * funcname, int line, const char * filename, const char * format, ...) __attribute__( (format(printf, 4, 5)) ); // todo
int PrintfError   (const char * funcname, int line, const char * filename, const char * format, ...) __attribute__( (format(printf, 4, 5)) ); // todo make single fucntion
int PrintfWarning (const char * funcname, int line, const char * filename, const char * format, ...) __attribute__( (format(printf, 4, 5)) ); // todo make single fucntion

#endif // TREE_H
