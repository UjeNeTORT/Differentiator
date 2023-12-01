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

// ============================================================================================

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

// ============================================================================================

TreeNode* TreeNodeCtor (double val, NodeType type, TreeNode * left, TreeNode * right)
{
    // ======================   CODE BLOCK    ======================
    TreeNode * new_node = (TreeNode *) calloc(1, sizeof(TreeNode));

    new_node->data  = {type, val};
    new_node->left  = left;
    new_node->right = right;

    return new_node;
}

// ============================================================================================

int TreeNodeDtor (TreeNode * node)
{
    // ======================   CODE BLOCK    ======================
    if (node) free(node);

    return 0; // return value in most cases is ignored
}

// ============================================================================================

Tree * TreeCtor ()
{
    // ======================   CODE BLOCK    ======================
    Tree * tree = (Tree *) calloc(1, sizeof(Tree));

    for(size_t i = 0; i < NAMETABLE_CAPACITY; i++)
    {
        tree->nametable.names[i] = (char *) calloc(MAX_OP, sizeof(char));
    }
    tree->nametable.free = 0;

    return tree;
}

// ============================================================================================

TreeDtorRes TreeDtor (Tree * tree)
{
    // ====================== ASSERTION BLOCK ======================
    assert(tree);

    if (!tree) RET_ERROR (TREE_DTOR_ERR_PARAMS, "Tree null pointer");

    // ======================   CODE BLOCK    ======================
    // traverse the tree and free each node
    TraverseTree(tree, TreeNodeDtor, POSTORDER);

    for(size_t i = 0; i < NAMETABLE_CAPACITY; i++)
    {
        free(tree->nametable.names[i]); //? do i even need this loop?
    }
    tree->nametable.free = 0;

    free(tree);

    return TREE_DTOR_SUCCESS;
}

// ============================================================================================

Tree* TreeCopy (const Tree * tree)
{
    // ====================== ASSERTION BLOCK ======================
    assert (tree);

    if (!tree) RET_ERROR (NULL, "Tree null pointer");

    // ======================   CODE BLOCK    ======================
    Tree* copied = TreeCtor ();
    copied->root = SubtreeCopy (tree->root);

    NameTableCopy (&copied->nametable, &tree->nametable);

    copied->size = tree->size;

    return copied;
}

// ============================================================================================

TreeNode * SubtreeCopy (TreeNode * node)
{
    // ======================   CODE BLOCK    ======================
    if (!node) return NULL;

    TreeNode * copied = TreeNodeCtor(node->data.val, node->data.type,
                                        SubtreeCopy(node->left), SubtreeCopy(node->right));

    return copied;
}

// ============================================================================================

NameTableCopyRes NameTableCopy (NameTable * dst, const NameTable * src)
{
    // ====================== ASSERTION BLOCK ======================
    assert(dst);
    assert(src);

    if (!dst) RET_ERROR(NAMETABLE_COPY_ERR_PARAMS, "Destination nametable null pointer");
    if (!dst) RET_ERROR(NAMETABLE_COPY_ERR_PARAMS, "Source nametable null pointer");

    // ======================   CODE BLOCK    ======================
    for (size_t i = 0; i < NAMETABLE_CAPACITY; i++)
    {
        dst->names[i] = strdup(src->names[i]);
        dst->vals[i]  = src->vals[i];
    }
    dst->free = src->free;

    return NAMETABLE_COPY_SUCCESS;
}

// ============================================================================================

TraverseTreeRes TraverseTreeFrom (Tree * tree, TreeNode * node, NodeAction_t NodeAction, TraverseOrder traverse_order)
{
    // ====================== ASSERTION BLOCK ======================
    assert(tree);
    if (!tree) RET_ERROR(TRVRS_TREE_ERR_PARAMS, "Tree null pointer");

    // ======================   CODE BLOCK    ======================
    if (!node) return TRVRS_TREE_SUCCESS;

    if (traverse_order == PREORDER)
    {
        NodeAction(node);
        TraverseTreeFrom(tree, node->left, NodeAction, traverse_order);
        TraverseTreeFrom(tree, node->right, NodeAction, traverse_order);
    }
    else if (traverse_order == INORDER)
    {
        TraverseTreeFrom(tree, node->left, NodeAction, traverse_order);
        NodeAction(node);
        TraverseTreeFrom(tree, node->right, NodeAction, traverse_order);
    }
    else if (traverse_order == POSTORDER)
    {
        TraverseTreeFrom(tree, node->left, NodeAction, traverse_order);
        TraverseTreeFrom(tree, node->right, NodeAction, traverse_order);
        NodeAction(node);
    }
    else
    {
        fprintf(stderr, "Wrong traversal order: %d\n", traverse_order);

        return TRVRS_TREE_ERR;
    }

    return TRVRS_TREE_SUCCESS;
}

// ============================================================================================

TraverseTreeRes TraverseTree (Tree * tree, NodeAction_t NodeAction, TraverseOrder traverse_order)
{
    assert(tree);

    return TraverseTreeFrom(tree, tree->root, NodeAction, traverse_order);
}

// ============================================================================================

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

// ============================================================================================

TreeNode * TreeFind (Tree * tree, double val, NodeType type)
{
    assert(tree);
    assert(val);

    return SubtreeFind (tree->root, val, type);
}

// ============================================================================================

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

        ABORT(); // !
    }

    TreeNode * node = TreeNodeCtor(0, NUM, NULL, NULL);
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

// ============================================================================================

Tree* ReadTree (const char * infix_tree)
{
    assert(infix_tree);

    int offset = 0;

    Tree* tree = TreeCtor();

    tree->root = ReadSubtree(infix_tree, &tree->nametable, &offset);

    return tree;
}

// ============================================================================================

Tree* ReadTree (FILE * stream)
{
    assert(stream);

    char * infix_tree = (char *) calloc(MAX_TREE, sizeof(char));

    fgets(infix_tree, MAX_TREE, stream);
    infix_tree[(strcspn(infix_tree, "\r\n"))] = 0;

    Tree* readen = ReadTree((const char *) infix_tree);

    free(infix_tree);

    return readen;
}

// ============================================================================================

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

    if (ReadAssignVariable(&data, word, nametable) == READ_ASSIGN_VAR_SUCCESS)
    {
        return data;
    }

    return data; // error NodeType
}

// ============================================================================================

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

// ============================================================================================

int WriteTree(FILE * stream, const Tree * tree)
{
    assert(stream);
    assert(tree);

    int ret_val = WriteSubtree(stream, tree->root, (const NameTable *) &tree->nametable);

    fprintf(stream, "\n");

    return ret_val;
}

// ============================================================================================

WriteNodeDataRes WriteNodeData (FILE * stream, NodeData data, const NameTable * nametable)
{
    assert(stream);
    assert(nametable);

    if (!nametable) RET_ERROR(WRT_NOD_DATA_ERR, "Nametable null pointer");
    if (!stream)    RET_ERROR(WRT_NOD_DATA_ERR, "Word null pointer");

    int opnum = -1;

    if (data.type == NUM)
    {
        fprintf(stream, "%lf ", data.val);

        return WRT_NOD_DATA_SUCCESS;
    }

    if (data.type == VAR)
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

    return WRT_NOD_DATA_SUCCESS;
}

// ============================================================================================

int UpdNameTable(NameTable * nametable, char * word)
{
    assert(nametable);
    assert(word);

    if (!nametable) RET_ERROR(-1, "Nametable null pointer");
    if (!word)      RET_ERROR(-1, "Word null pointer");

    strcpy(nametable->names[nametable->free], word);

    return nametable->free++;
}

int IncorrectVarName (const char * word)
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

// ============================================================================================

ReadAssignVariableRes ReadAssignVariable (NodeData * data, char * word, NameTable * nametable)
{
    assert(data);
    assert(word);

    if (!data) RET_ERROR(READ_ASSIGN_VAR_ERR_PARAMS, "data null pointer");
    if (!word) RET_ERROR(READ_ASSIGN_VAR_ERR_PARAMS, "word null pointer");

    if (IncorrectVarName((const char *) word))
        RET_ERROR(READ_ASSIGN_VAR_ERR, "Incorrect variable name \"%s\"\n", word);

    int var_id = -1;

    if ((var_id = FindNametableDups(nametable, word)) == -1) // no dups
    {
        var_id = UpdNameTable(nametable, word);

        if (!ScanVariableVal(nametable, var_id)) return READ_ASSIGN_VAR_ERR;
    }

    data->type = VAR;
    data->val  = var_id;

    return READ_ASSIGN_VAR_SUCCESS;
}

// ============================================================================================

ReadAssignOperatorRes ReadAssignOperator (NodeData * data, char * word)
{
    // ====================== ASSERTION BLOCK ======================
    assert(data);
    assert(word);

    if (!data) RET_ERROR(READ_ASSIGN_OP_ERR_PARAMS, "data null pointer");
    if (!word) RET_ERROR(READ_ASSIGN_OP_ERR_PARAMS, "word null pointer");

    // ======================   CODE BLOCK    ======================
    for (size_t i = 0; i < OPERATIONS_NUM; i++)
    {
        if (streq(word, OPERATIONS[i].name))
        {
            data->type = OPERATIONS[i].type;
            data->val  = OPERATIONS[i].opcode;

            return READ_ASSIGN_OP_SUCCESS;
        }
    }

    return READ_ASSIGN_OP_ERR_NOT_FOUND;
}

// ============================================================================================

ReadAssignDoubleRes ReadAssignDouble (NodeData * data, char * word)
{
    // ====================== ASSERTION BLOCK ======================
    assert(data);
    assert(word);

    if (!data) RET_ERROR(READ_ASSIGN_DBL_ERR_PARAMS, "data null pointer");
    if (!word) RET_ERROR(READ_ASSIGN_DBL_ERR_PARAMS, "word null pointer");

    // ======================   CODE BLOCK    ======================
    if (IsDouble(word)) // ! may be unsafe see function code
    {
        data->type = NUM;
        data->val = atof(word);

        return READ_ASSIGN_DBL_SUCCESS; // assigned double to data
    }

    return READ_ASSIGN_DBL_ERR; // didnt assign double to data
}

// ============================================================================================

int ScanVariableVal (NameTable * nametable, int var_id)
{
    // ====================== ASSERTION BLOCK ======================
    assert(nametable);

    if (!nametable) RET_ERROR(0, "Nametable null pointer");

    // ======================   CODE BLOCK    ======================
    fprintf(stdout, "Please specify variable \"%s\"\n>> ", nametable->names[var_id]);

    return fscanf(stdin, "%lf", &nametable->vals[var_id]);
}

// ============================================================================================

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

// ============================================================================================

int FindNametableDups (NameTable * nametable, const char * word)
{
    // ====================== ASSERTION BLOCK ======================
    assert (nametable);
    assert (word);

    if (!nametable) RET_ERROR (ILL_OPNUM, "Nametable null pointer");
    if (!word)      RET_ERROR (ILL_OPNUM, "Word null pointer");

    // ======================   CODE BLOCK    ======================
    for (size_t i = 0; i < NAMETABLE_CAPACITY; i++)
    {
        if (streq (nametable->names[i], word))
            return i; // duplicate id
    }

    return -1; // no duplicates
}

int IsDouble (char * word)
{
    // may be unsafe, not tested properly
    // it handles most used cases

    // ====================== ASSERTION BLOCK ======================
    assert (word);

    if (!word) RET_ERROR (0, "Word null pointer");

    // ======================   CODE BLOCK    ======================
    if (IsZero (atof (word)) && *word != '0' && *word != '.')
    {
        return 0;
    }

    return 1;
}

int IsZero (double num)
{
    return abs (num) < EPS ? 1 : 0;
}

