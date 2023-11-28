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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../colors.h"
#include "tree.h"

double Eval (const TreeNode * node, const NameTable * nametable)
{
    assert(node);

    // todo change assert to if !node

    double left  = 0;
    double right = 0;

    switch (node->data.type)
    {

    case NUM:
        return node->data.val;

    case VAR:
        return nametable->vals[(int) node->data.val];

    case UN_OP:
        right = Eval(node->right, nametable);

        switch ((int) node->data.val)
        {
        case EXP:
            return pow(EXPONENT, right);
        case LN:
            return log(right);
        case SIN:
            return sin(right);
        case COS:
            return cos(right);
        case TG:
            return tan(right);
        case CTG:
            return 1 / tan(right);
        case ASIN:
            return asin(right);
        case ACOS:
            return acos(right);
        case ATG:
            return atan(right);
        case ACTG:
            return PI / 2 - atan(right);
        case SH:
            return sinh(right);
        case CH:
            return cosh(right);
        case TH:
            return tanh(right);
        case CTH:
            return 1 / tanh(right);

        default: // ! not finished
            // todo
            break;
        }

        return 0;

    case BI_OP:
        left  = Eval(node->left, nametable);
        right = Eval(node->right, nametable);

        switch ((int) node->data.val)
        {
        case ADD:
            return left + right;
        case SUB:
            return left - right;
        case MUL:
            return left * right;
        case DIV:
            return left / right;
        case POW:
            return pow(left,right);
        default: // ! not finished
            // todo
            break;
        }

        return 0;
    }

    return 0;
}

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

    for(size_t i = 0; i < NAMETABLE_CAPACITY; i++)
    {
        tree.nametable.names[i] = (char *) calloc(MAX_OP, sizeof(char));
    }
    tree.nametable.free = 0;

    return tree;
}

int TreeDtor (Tree * tree)
{
    assert(tree);

    // traverse the tree and free each node
    TraverseTree(tree, TreeNodeDtor, POSTORDER);

    for(size_t i = 0; i < NAMETABLE_CAPACITY; i++)
    {
        free(tree->nametable.names[i]); //? do i even need this loop?
    }
    tree->nametable.free = 0;

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

Tree TreeCopy (Tree * tree)
{
    assert(tree);

    Tree copied = TreeCtor(NULL);
    copied.root = SubtreeCopy(tree->root);

    for (size_t i = 0; i < NAMETABLE_CAPACITY; i++)
    {
        memcpy(copied.nametable.names[i], tree->nametable.names[i], sizeof(tree->nametable.names[i]));
        copied.nametable.vals[i] = tree->nametable.vals[i];
    }
    copied.nametable.free = tree->nametable.free;

    copied.size = tree->size;

    return copied;
}

TreeNode * SubtreeCopy (TreeNode * node)
{
    if (!node) return NULL;

    TreeNode * copied = TreeNodeCtor(node->data.val, node->data.type);

    copied->left  = SubtreeCopy(node->left);
    copied->right = SubtreeCopy(node->right);

    return copied;
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

TreeNode * ReadSubtree (const char * infix_tree, NameTable * nametable, int * offset)
{
    assert(infix_tree);

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
    *offset += 1; // skip (

    node->left = ReadSubtree(infix_tree, nametable, offset);

    node->data = ReadNodeData(infix_tree, nametable, offset);

    node->right = ReadSubtree(infix_tree, nametable, offset);

    while(infix_tree[*offset] != ')')
    {
        (*offset)++;
    }

    (*offset)++;

    return node;
}

Tree ReadTree (const char * infix_tree)
{
    assert(infix_tree);

    int offset = 0;

    Tree tree = TreeCtor(NULL);

    tree.root = ReadSubtree(infix_tree, &tree.nametable, &offset);

    return tree;
}

Tree ReadTree (FILE * stream)
{
    assert(stream);

    char * infix_tree = (char *) calloc(MAX_TREE, sizeof(char));

    fgets(infix_tree, MAX_TREE, stream);
    infix_tree[(strcspn(infix_tree, "\r\n"))] = 0;

    Tree readen = ReadTree((const char *) infix_tree);

    free(infix_tree);

    return readen;
}

NodeData ReadNodeData(const char * infix_tree, NameTable * nametable, int * offset)
{
    assert(infix_tree);
    assert(offset);

    NodeData data = {ERR, 0};

    while(isspace(infix_tree[*offset]))
    {
        *offset += 1;
    }

    char word[MAX_OP] = "";

    int addition = 0;

    sscanf((infix_tree + *offset), "%s%n", word, &addition); // todo what if var name longer than MAX_OP

    *offset += addition;

    if (ReadAssignDouble(&data, word))
    {
        return data;
    }

    if (ReadAssignOperator(&data, word))
    {
        return data;
    }

    if (ReadAssignVariable(&data, word, nametable))
    {
        return data;
    }

    return data; // error NodeType
}

int WriteSubtree(FILE * stream, const TreeNode * node, const NameTable * nametable)
{
    assert(stream);

    if (!node)
    {
        fprintf(stream, "* ");

        return 0;
    }

    fprintf(stream, "( ");

    WriteSubtree(stream, node->left, nametable);
    int ret_val = WriteNodeData(stream, node->data, nametable);
    WriteSubtree(stream, node->right, nametable);

    fprintf(stream, ") ");

    return ret_val;
}

int WriteTree(FILE * stream, const Tree * tree)
{
    assert(stream);
    assert(tree);

    int ret_val = WriteSubtree(stream, tree->root, (const NameTable *) &tree->nametable);

    fprintf(stream, "\n");

    return ret_val;
}

int WriteNodeData(FILE * stream, NodeData data, const NameTable * nametable)
{
    assert(stream);

    int opnum = -1;

    if (data.type == NUM)
    {
        fprintf(stream, "%lf ", data.val);

        return 0;
    }
    else if (data.type == VAR)
    {
        fprintf(stream, "%s ", nametable->names[(int) data.val]);
    }
    else if (data.type == BI_OP || data.type == UN_OP)
    {
        opnum = FindOperation((int) data.val);
        if (opnum != ILL_OPNUM)
            fprintf(stream, " %s ", OPERATIONS[opnum].name);
        else
            fprintf(stream, "UNKNOWN OPERATOR");
    }

    return 0;
}

int UpdNameTable(NameTable * nametable, char * word)
{
    assert(word);

    strcpy(nametable->names[nametable->free], word);
    return nametable->free++;
}

int IncorrectVarName(const char * word)
{
    assert(word);

    if (!isalpha(*word++))
    {
        return 1;
    }

    while (*word)
    {
        if (!isalnum(*word) && *word != '_')
        {
            return 1;
        }

        word++;
    }

    return 0;
}

int ReadAssignVariable (NodeData * data, char * word, NameTable * nametable)
{
    assert(data);
    assert(word);

    if (IncorrectVarName((const char *) word))
    {
        fprintf(stderr, "ReadNodeData: incorrect variable name \"%s\"\n", word);

        return 0;
    }

    int var_id = -1;

    if ((var_id = FindNametableDups(nametable, word)) == -1) // no dups
    {
        var_id = UpdNameTable(nametable, word);

        if (!ScanVariableVal(nametable, var_id))
        {
            return 0; // error code not documented
        }
    }

    data->type = VAR;
    data->val  = var_id;

    return 1;
}

int ReadAssignOperator (NodeData * data, char * word)
{
    assert(data);
    assert(word);

    for (size_t i = 0; i < OPERATIONS_NUM; i++)
    {
        if (streq(word, OPERATIONS[i].name))
        {
            data->type = OPERATIONS[i].type;
            data->val  = OPERATIONS[i].opcode;

            return 1;
        }
    }

    return 0;
}

int ReadAssignDouble (NodeData * data, char * word)
{
    assert(data);
    assert(word);

    if (IsDouble(word)) // ! WARNING CRUTCH ! check if word represents double
    {
        data->type = NUM;
        data->val = atof(word);

        return 1; // assigned double to data
    }

    return 0; // didnt assign double to data
}

int ScanVariableVal (NameTable * nametable, int var_id)
{
    assert(nametable);

    fprintf(stdout, "Please specify variable \"%s\"\n>> ", nametable->names[var_id]);

    return fscanf(stdin, "%lf", &nametable->vals[var_id]);
}

int FindOperation (int opcode)
{
    for (int i = 0; i < OPERATIONS_NUM; i++)
    {
        if (opcode == OPERATIONS[i].opcode)
        {
            return i;
        }
    }

    return ILL_OPNUM;
}

int FindNametableDups(NameTable * nametable, const char * word)
{
    assert(nametable);
    assert(word);

    for (size_t i = 0; i < NAMETABLE_CAPACITY; i++)
    {
        if (streq(nametable->names[i], word))
            return i; // duplicate id
    }

    return -1; // no duplicates
}

int IsDouble (char * word) // ! WARNING crutch function
{
    assert(word);

    if (IsZero(atof(word)) && *word != '0' && *word != '.')
    {
        return 0;
    }

    return 1;
}

int IsZero (double num)
{
    return abs(num) < EPS ? 1 : 0;
}

int PrintfDebug (const char * funcname, int line, const char * filename, const char * format, ...)
{
    assert(format);

    fprintf(stderr, BLACK_CLR "[DEBUG MESSAGE %s %d %s]\n<< ", funcname, line, filename);

    va_list ptr;

    va_start(ptr, format);

    int res = vfprintf(stderr, format, ptr);

    va_end(ptr);

    fprintf(stdout, RST_CLR "\n" );

    return res;
}
