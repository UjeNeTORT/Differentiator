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

#define streq(s1, s2) (!strcmp((s1), (s2)))

#define PRINTF_DEBUG(format, ...) \
    PrintfDebug (__PRETTY_FUNCTION__, __LINE__, __FILE__, format __VA_OPT__(,) __VA_ARGS__)

const int MAX_TREE = 1000;
const int MAX_OP = 10;

typedef enum
{
    EQUAL = 0,
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4,
    POW = 5,
} Opcodes;

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
    NUM   = 0, // number
    VAR   = 1, // variable
    BI_OP = 2, // binary operator
    UN_OP = 3, // unary operator
} NodeType;

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

    int size;
};

typedef int (* NodeAction_t) (TreeNode * node);

TreeNode* TreeNodeCtor (double val, NodeType type);
int       TreeNodeDtor (TreeNode * node);

Tree      TreeCtor     (TreeNode * root);
int       TreeDtor     (Tree * tree);

int       TreeHangNode (Tree * tree, TreeNode * node, TreeNode * new_node, NodeLocation new_node_location, NodeLocation subtree_location);

int       DeleteSubtree (Tree * tree, TreeNode * node);

int       TraverseTree     (Tree * tree, NodeAction_t NodeAction, TraverseOrder traverse_order);
int       TraverseTreeFrom (Tree * tree, TreeNode * node, NodeAction_t NodeAction, TraverseOrder traverse_order);

TreeNode * SubtreeFind (TreeNode * node, double val, NodeType type);
TreeNode * TreeFind (Tree * tree, double val, NodeType type);

TreeNode * ReadSubtree (const char * infix_tree);
Tree       ReadTree (const char * infix_tree);
NodeData   ReadNodeData(const char * infix_tree, int * offset);

int WriteSubtree(FILE * stream, const TreeNode * node);
int WriteTree(FILE * stream, const Tree * tree);
int WriteNodeData(FILE * stream, NodeData data);

int       PrintfDebug (const char * funcname, int line, const char * filename, const char * format, ...) __attribute__( (format(printf, 4, 5)) );

#endif // TREE_H
