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

#define streq(s1, s2) (!strcmp((s1), (s2)))

#define PRINTF_DEBUG(format, ...) \
    PrintfDebug (__FUNCTION__, __LINE__, __FILE__, format __VA_OPT__(,) __VA_ARGS__)

#define PRINTF_ERROR(format, ...) \
    PrintfError (__FUNCTION__, __LINE__, __FILE__, format __VA_OPT__(,) __VA_ARGS__)

const int MAX_TREE = 5000;
const int MAX_OP   = 10; // max len of operator or variable name // todo rename

const int NAMETABLE_CAPACITY = 10;

const double EPS = 1e-7;

const double EXPONENT = 2.718281828;
const double PI       = 3.141592654;

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

TreeNode* TreeNodeCtor (double val, NodeType type);
int       TreeNodeDtor (TreeNode * node);

Tree      TreeCtor     (TreeNode * root);
int       TreeDtor     (Tree * tree);

int       TreeHangNode (Tree * tree, TreeNode * node, TreeNode * new_node, NodeLocation new_node_location, NodeLocation subtree_location);

Tree       TreeCopy    (Tree * tree);
TreeNode * SubtreeCopy (TreeNode * node);

int       TraverseTree     (Tree * tree, NodeAction_t NodeAction, TraverseOrder traverse_order);
int       TraverseTreeFrom (Tree * tree, TreeNode * node, NodeAction_t NodeAction, TraverseOrder traverse_order);

TreeNode * SubtreeFind (TreeNode * node, double val, NodeType type);
TreeNode * TreeFind    (Tree * tree, double val, NodeType type);

Tree       ReadTree     (FILE * stream);
Tree       ReadTree     (const char * infix_tree);
TreeNode * ReadSubtree  (const char * infix_tree, NameTable * nametable, int * offset);
NodeData   ReadNodeData (const char * infix_tree, NameTable * nametable, int * offset);

int WriteSubtree  (FILE * stream, const TreeNode * node, const NameTable * nametable);
int WriteTree     (FILE * stream, const Tree * tree);
int WriteNodeData (FILE * stream, NodeData data, const NameTable * nametable);

int UpdNameTable     (NameTable * nametable, char * word);
int IncorrectVarName (const char * word);

int ReadAssignVariable (NodeData * data, char * word, NameTable * nametable);
int ReadAssignOperator (NodeData * data, char * word);
int ReadAssignDouble   (NodeData * data, char * word);

int ScanVariableVal (NameTable * nametable, int var_id);

int FindOperation (int opcode);

int FindNametableDups(NameTable * nametable, const char * word);

int IsDouble (char * word); // ! WARNING cructh function

int IsZero (double num);

int PrintfDebug (const char * funcname, int line, const char * filename, const char * format, ...) __attribute__( (format(printf, 4, 5)) ); // todo
int PrintfError (const char * funcname, int line, const char * filename, const char * format, ...) __attribute__( (format(printf, 4, 5)) ); // todo make single fucntion

#endif // TREE_H
