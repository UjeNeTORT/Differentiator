/*************************************************************************
 * (c) 2023 Tikhonov Yaroslav (aka UjeNeTORT)
 *
 * email: tikhonovty@gmail.com
 * telegram: https://t.me/netortofficial
 * GitHub:   https://github.com/UjeNeTORT
 * repo:     https://github.com/UjeNeTORT/Differentiator
 *************************************************************************/

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../colors.h"
#include "tree.h"

TreeNode* TreeNodeCtor (double val, NodeType type)
{
    TreeNode * new_node = (TreeNode *) calloc(1, sizeof(TreeNode));

    new_node->data  = {type, val};
    new_node->left  = NULL;
    new_node->right = NULL;

    return new_node;
}

int TreeNodeDtor (TreeNode * node)
{
    if (!node)
    {
        return 0;
    }

    free(node);

    return 0;
}

Tree TreeCtor (TreeNode * root)
{
    Tree tree = { root, 0 };

    return tree;
}

int TreeDtor (Tree * tree)
{
    assert(tree);

    // traverse the tree and free each node
    TraverseTree(tree, TreeNodeDtor, POSTORDER);

    return 0;
}

int TreeHangNode (Tree * tree, TreeNode * node, TreeNode * new_node, NodeLocation new_node_location, NodeLocation subtree_location)
{
    assert(tree);

    int ret_val = 0;

    if (!node && !tree->root)
    {
        if (tree->size == 0) // new node is tree root then
        {
            tree->root = new_node;

            tree->size++;

            return ret_val;
        }
        else
        {
            ret_val = -1; // error code

            return ret_val;
        }
    }

    TreeNode * subtree_ptr = NULL;

    if (new_node_location == LEFT)
    {
        subtree_ptr = node->left;
        node->left  = new_node;
    }
    else if (new_node_location == RIGHT)
    {
        subtree_ptr = node->right;
        node->right = new_node;
    }
    else if (new_node_location == REPLACE)
    {
        subtree_ptr = node->left;
        new_node->left = node->left;
        new_node->right = node->right;

        *node = *new_node;
    }

    if (new_node_location == REPLACE)
    {
        // do nothing
    }
    else if (subtree_location == LEFT)
    {
        new_node->left = subtree_ptr;
    }
    else if (subtree_location == RIGHT)
    {
        new_node->right = subtree_ptr;
    }
    else if (subtree_location == REPLACE) // we forbid such fucntionality
    {
        fprintf(stderr, "Forbidden to hang subtree onto intself. Choose LEFT or RIGHT\n");

        ret_val = 1;
    }

    tree->size++;

    return ret_val;
}

int TraverseTreeFrom (Tree * tree, TreeNode * node, NodeAction_t NodeAction, TraverseOrder traverse_order)
{
    assert(tree);

    if (!node)
    {
        return 0;
    }

    int ret_val = 0;

    if (traverse_order == PREORDER)
    {
        ret_val = NodeAction(node);
        TraverseTreeFrom(tree, node->left, NodeAction, traverse_order);
        TraverseTreeFrom(tree, node->right, NodeAction, traverse_order);
    }
    else if (traverse_order == INORDER)
    {
        TraverseTreeFrom(tree, node->left, NodeAction, traverse_order);
        ret_val = NodeAction(node);
        TraverseTreeFrom(tree, node->right, NodeAction, traverse_order);
    }
    else if (traverse_order == POSTORDER)
    {
        TraverseTreeFrom(tree, node->left, NodeAction, traverse_order);
        TraverseTreeFrom(tree, node->right, NodeAction, traverse_order);
        ret_val = NodeAction(node);
    }
    else
    {
        fprintf(stderr, "Wrong traversal order: %d\n", traverse_order);

        return 1;
    }

    return ret_val;
}

int TraverseTree (Tree * tree, NodeAction_t NodeAction, TraverseOrder traverse_order)
{
    assert(tree);

    return TraverseTreeFrom(tree, tree->root, NodeAction, traverse_order);
}

TreeNode * SubtreeFind (TreeNode * node, double val, NodeType type)
{
    assert(val);

    if (!node)
    {
        return NULL;
    }

    if (node->data.val == val && node->data.type == type)
    {
        return node;
    }

    TreeNode * res = NULL;

    return ((res = SubtreeFind(node->left, val, type)) == NULL) ? SubtreeFind(node->right, val, type) : res;
}

TreeNode * TreeFind (Tree * tree, double val, NodeType type)
{
    assert(tree);
    assert(val);

    return SubtreeFind (tree->root, val, type);
}

TreeNode * ReadSubtree (const char * infix_tree, int * offset)
{
    assert(infix_tree);

    PRINTF_DEBUG("received: %s", infix_tree + *offset);

    while(isspace(infix_tree[*offset]))
    {
        *offset += 1;
    }

    if (infix_tree[*offset] == '*')
    {
        *offset += 1;

        return NULL;
    }
    else if (infix_tree[*offset] != '(')
    {
        fprintf(stderr, "ReadSubTree: unknown action symbol %c (%d)\n", infix_tree[*offset], infix_tree[*offset]);

        abort();
    }

    TreeNode * node = TreeNodeCtor(0, NUM);
    *offset += 1;
    PRINTF_DEBUG("node created\n");
    node->left = ReadSubtree(infix_tree, offset);
    PRINTF_DEBUG("left subtree readen\n");
    node->data = ReadNodeData(infix_tree, offset);
    PRINTF_DEBUG("right subtree readen\n");
    node->right = ReadSubtree(infix_tree, offset);

    while(infix_tree[(*offset)++] != ')')
    {
        ;
    }

    return node;
}

Tree ReadTree (const char * infix_tree)
{
    assert(infix_tree);

    int offset = 0;

    return TreeCtor(ReadSubtree(infix_tree, &offset));
}

NodeData ReadNodeData(const char * infix_tree, int * offset)
{
    assert(infix_tree);
    assert(offset);

    NodeData data = {NUM, 0};

    while(isspace(infix_tree[*offset]))
    {
        *offset += 1;
    }

    char op_or_num[MAX_OP] = "";

    int addition = 0;

    sscanf((infix_tree + *offset), "%s%n", op_or_num, &addition);
    PRINTF_DEBUG("op_or_num = %s\n", op_or_num);

    *offset += addition;

    if (data.val = atof(op_or_num))
    {
        return data;
    }

    if (streq(op_or_num, "="))
    {
        data.type = UN_OP;
        data.val = EQUAL;
    }
    else if (streq(op_or_num, "+"))
    {
        data.type = BI_OP;
        data.val = ADD;
    }
    else if (streq(op_or_num, "-"))
    {
        data.type = BI_OP;
        data.val = SUB;
    }
    else if (streq(op_or_num, "*"))
    {
        data.type = BI_OP;
        data.val = MUL;
    }
    else if (streq(op_or_num, "/"))
    {
        data.type = BI_OP;
        data.val = DIV;
    }
    else if (streq(op_or_num, "^"))
    {
        data.type = BI_OP;
        data.val = POW;
    }
    else
    {
        fprintf(stdout, "Unknown operator: %s\n", op_or_num);

        abort();
    }

    return data;
}

int WriteSubtree(FILE * stream, const TreeNode * node)
{
    assert(stream);

    if (!node)
    {
        fprintf(stream, "* ");

        return 0;
    }

    fprintf(stream, "( ");

    WriteSubtree(stream, node->left);
    int ret_val = WriteNodeData(stream, node->data);
    WriteSubtree(stream, node->right);

    fprintf(stream, ") ");

    return ret_val;
}

int WriteTree(FILE * stream, const Tree * tree)
{
    assert(stream);
    assert(tree);

    int ret_val = WriteSubtree(stream, tree->root);

    fprintf(stream, "\n");

    return ret_val;
}

int WriteNodeData(FILE * stream, NodeData data)
{
    assert(stream);

    if (data.type == NUM)
    {
        fprintf(stream, "%lf ", data.val);

        return 0;
    }
    else if (data.type == BI_OP || data.type == UN_OP)
    {
        // ! cringe code
        switch ((int) data.val)
        {
        case EQUAL:
            fprintf(stream, "= ");

            break;

        case ADD:
            fprintf(stream, "+ ");

            break;

        case SUB:
            fprintf(stream, "- ");

            break;

        case MUL:
            fprintf(stream, "* ");

            break;

        case DIV:
            fprintf(stream, "/ ");

            break;

        case POW:
            fprintf(stream, "^ ");

            break;

        default:
            fprintf(stream, "UNKNOWN OPERATOR ");

            break;
        }
    }

    return 0;
}

int PrintfDebug (const char * funcname, int line, const char * filename, const char * format, ...)
{
    assert(format);

    fprintf(stderr, BLACK_CLR "[DEBUG MESSAGE %s %d %s]\n<< ", funcname, line, filename);

    va_list ptr;

    va_start(ptr, format);

    int res = vfprintf(stderr, format, ptr);

    va_end(ptr);

    fprintf(stdout, "\n " RST_CLR);

    return res;
}
