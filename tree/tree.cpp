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

#include "tree.h"

// ============================================================================================

TreeEvalRes TreeEval (Tree* tree, double* result)
{
    assert(tree);
    assert(result);
    if (!tree)   RET_ERROR(TREE_EVAL_ERR_PARAMS, "Tree null pointer");
    if (!result) RET_ERROR(TREE_EVAL_ERR_PARAMS, "Result null pointer");

    return SubtreeEval (tree->root, tree, result);
}

// ============================================================================================

TreeEvalRes SubtreeEval (TreeNode* node, const Tree* tree, double* result)
{
    assert(tree);
    assert(result);
    if (!tree)   RET_ERROR(TREE_EVAL_ERR_PARAMS, "Tree null pointer");
    if (!result) RET_ERROR(TREE_EVAL_ERR_PARAMS, "Result null pointer");

    if (!node) return TREE_EVAL_SUCCESS;

    double left  = 0;
    double right = 0;

    TreeEvalRes ret_val = TREE_EVAL_SUCCESS;

    switch (TYPE(node))
    {

    case NUM:
        *result = VAL(node);
        break;

    case VAR:
        *result = tree->nametable.vals[(int) VAL(node)];
        break;

    case UN_OP:
        if (SubtreeEval(node->right, tree, &right) != TREE_EVAL_SUCCESS)
            RET_ERROR(TREE_EVAL_ERR, "Previous function returned error code");

        ret_val = SubtreeEvalUnOp(node, right, result);
        break;

    case BI_OP:
        ret_val = SubtreeEval (node->left, tree, &left);
        if (ret_val != TREE_EVAL_SUCCESS) RET_ERROR(TREE_EVAL_ERR, "Previous function returned error code");
        ret_val = SubtreeEval (node->right, tree, &right);
        if (ret_val != TREE_EVAL_SUCCESS) RET_ERROR(TREE_EVAL_ERR, "Previous function returned error code");

        ret_val = SubtreeEvalBiOp (node, left, right, result);
        break;

    default:
        RET_ERROR(TREE_EVAL_FORBIDDEN, "Unknown node type %d", TYPE(node));
    }

    return ret_val;
}

// ============================================================================================

TreeEvalRes SubtreeEvalNums (TreeNode * node, double* result)
{
    if (!node) return TREE_EVAL_SUCCESS;

    double left  = 0;
    double right = 0;

    TreeEvalRes ret_val = TREE_EVAL_SUCCESS;

    switch (TYPE(node))
    {

    case NUM:
        *result = VAL(node);
        break;

    case UN_OP:
        if (SubtreeEvalNums(node->right, &right) != TREE_EVAL_SUCCESS)
            RET_ERROR(TREE_EVAL_ERR, "Previous function returned error code");

        ret_val = SubtreeEvalUnOp(node, right, result);
        break;

    case BI_OP:
        if (SubtreeEvalNums(node->left, &left) != TREE_EVAL_SUCCESS)
            RET_ERROR(TREE_EVAL_ERR, "Previous function returned error code");

        if (SubtreeEvalNums(node->right, &right) != TREE_EVAL_SUCCESS)
            RET_ERROR(TREE_EVAL_ERR, "Previous function returned error code");

        ret_val = SubtreeEvalBiOp(node, left, right, result);
        break;

    default:
        RET_ERROR(TREE_EVAL_FORBIDDEN, "Only nodes manipulating with numbers allowed (received node type = %d)", TYPE(node));
    }

    return ret_val;
}

// ============================================================================================

TreeEvalRes SubtreeEvalUnOp (TreeNode* node, double right, double* result)
{
    assert(result);
    if (!result) return TREE_EVAL_ERR_PARAMS;
    if (TYPE(node) != UN_OP) RET_ERROR(TREE_EVAL_ERR_PARAMS, "You can not pass anything but BI_OP node to this function");

    if (!node) return TREE_EVAL_SUCCESS;

    switch ((int) VAL(node))
    {
    case EQUAL:
        *result = right;
        break;

    case EXP:
        *result = pow(EXPONENT, right);
        break;

    case LN:
        *result = log(right);
        break;

    case SIN:
        *result = sin(right);
        break;

    case COS:
        *result = cos(right);
        break;

    case TG:
        *result = tan(right);
        break;

    case CTG:
        *result = 1 / tan(right);
        break;

    case ASIN:
        *result = asin(right);
        break;

    case ACOS:
        *result = acos(right);
        break;

    case ATG:
        *result = atan(right);
        break;

    case ACTG:
        *result = PI / 2 - atan(right);
        break;

    case SH:
        *result = sinh(right);
        break;

    case CH:
        *result = cosh(right);
        break;

    case TH:
        *result = tanh(right);
        break;

    case CTH:
        *result = 1 / tanh(right);
        break;

    default:
        RET_ERROR(TREE_EVAL_ERR, "Unknown operator: %d", (int) VAL(node));
    }

    return TREE_EVAL_SUCCESS;
}

// ============================================================================================

TreeEvalRes SubtreeEvalBiOp (TreeNode* node, double left, double right, double* result)
{
    assert(result);
    if (!result) return TREE_EVAL_ERR_PARAMS;
    if (TYPE(node) != BI_OP) RET_ERROR(TREE_EVAL_ERR_PARAMS, "You can not pass anything but BI_OP node to this function");

    if (!node) return TREE_EVAL_SUCCESS;

    switch ((int) VAL(node))
    {
    case ADD:
        *result = left + right;
        break;

    case SUB:
        *result = left - right;
        break;

    case MUL:
        *result = left * right;
        break;

    case DIV:
        *result = left / right;
        break;

    case POW:
        *result = pow(left,right);
        break;

    default:
        RET_ERROR(TREE_EVAL_ERR, "Unknown operator: %d", (int) VAL(node));
    }

    return TREE_EVAL_SUCCESS;
}

// ============================================================================================

TreeSimplifyRes TreeSimplify (Tree* tree)
{
    assert(tree);
    if (!tree) RET_ERROR(TREE_SIMLIFY_ERR_PARAMS, "Tree null pointer");

    TreeSimplifyRes ret_val = TREE_SIMPLIFY_SUCCESS;

    int tree_changed_flag = 0;

    do
    {
        tree_changed_flag = 0;

        ret_val = TreeSimplifyConstants (tree, &tree_changed_flag);
        if (ret_val != TREE_SIMPLIFY_SUCCESS) break;

        ret_val = TreeSimplifyNeutrals  (tree, &tree_changed_flag);
        if (ret_val != TREE_SIMPLIFY_SUCCESS) break;

    } while (tree_changed_flag);

    return ret_val;
}

// ============================================================================================

TreeSimplifyRes TreeSimplifyConstants (Tree* tree, int* tree_changed_flag)
{
    assert(tree);
    assert(tree_changed_flag);
    if (!tree) RET_ERROR(TREE_SIMLIFY_ERR_PARAMS, "Tree null pointer");
    if (!tree_changed_flag) RET_ERROR(TREE_SIMLIFY_ERR_PARAMS, "Tree changed flag null pointer");

    TreeSimplifyRes ret_val = TREE_SIMPLIFY_SUCCESS;

    int local_tree_changed_flag = 0;

    do
    {
        local_tree_changed_flag = 0;
        ret_val = SubtreeSimplifyConstants (tree->root, &local_tree_changed_flag);

        if (ret_val != TREE_SIMPLIFY_SUCCESS) return ret_val;
    } while (local_tree_changed_flag);

    if (local_tree_changed_flag) *tree_changed_flag = 1;

    return ret_val;
}

// ============================================================================================

TreeSimplifyRes TreeSimplifyNeutrals (Tree* tree, int* tree_changed_flag)
{
    assert(tree);
    assert(tree_changed_flag);
    if (!tree) RET_ERROR(TREE_SIMLIFY_ERR_PARAMS, "Tree null pointer");
    if (!tree_changed_flag) RET_ERROR(TREE_SIMLIFY_ERR_PARAMS, "Tree changed flag null pointer");

    TreeSimplifyRes ret_val = TREE_SIMPLIFY_SUCCESS;

    int local_tree_changed_flag = 0;

    do
    {
        local_tree_changed_flag = 0;
        ret_val = SubtreeSimplifyNeutrals (tree->root, tree_changed_flag);

        if (ret_val != TREE_SIMPLIFY_SUCCESS) return ret_val;
    } while (local_tree_changed_flag);

    if (local_tree_changed_flag) *tree_changed_flag = 1;

    return ret_val;
}

// ============================================================================================

TreeSimplifyRes SubtreeSimplifyConstants (TreeNode* node, int* tree_changed_flag)
{
    assert(tree_changed_flag);
    if (!tree_changed_flag)
               RET_ERROR(TREE_SIMLIFY_ERR_PARAMS, "flag null pointer");

    if (!node) return TREE_SIMPLIFY_SUCCESS;
    if (TYPE(node) == NUM) return TREE_SIMPLIFY_SUCCESS;
    if (TYPE(node) == VAR) return TREE_SIMPLIFY_SUCCESS;

    TreeSimplifyRes ret_val = TREE_SIMPLIFY_SUCCESS;

    ret_val = SubtreeSimplifyConstants (node->left, tree_changed_flag);
    if (ret_val != TREE_SIMPLIFY_SUCCESS)
        RET_ERROR (TREE_SIMPLIFY_ERR, "Previous function returned error code %d", ret_val);

    ret_val = SubtreeSimplifyConstants (node->right, tree_changed_flag);
    if (ret_val != TREE_SIMPLIFY_SUCCESS)
        RET_ERROR (TREE_SIMPLIFY_ERR, "Previous function returned error code %d", ret_val);

    if (TYPE(node) == BI_OP && TYPE(node->left) == NUM && TYPE(node->right) == NUM)
    {
        if (SubtreeEvalNums (node, &VAL(node)) != TREE_EVAL_SUCCESS) return TREE_SIMPLIFY_ERR;
        TYPE(node) = NUM;

        TreeNodeDtor(node->left);
        TreeNodeDtor(node->right);
        node->left  = NULL;
        node->right = NULL;

        *tree_changed_flag = 1;

        return ret_val;
    }

    if (TYPE(node) == UN_OP && TYPE(node->right) == NUM)
    {
        if (SubtreeEvalNums (node, &VAL(node)) != TREE_EVAL_SUCCESS) return TREE_SIMPLIFY_ERR;
        TYPE(node) = NUM;

        TreeNodeDtor(node->left);

        node->left = NULL;

        *tree_changed_flag = 1;

        return ret_val;
    }

    return ret_val;
}

// ============================================================================================

TreeSimplifyRes SubtreeSimplifyNeutrals  (TreeNode* node, int* tree_changed_flag)
{
    assert(tree_changed_flag);

    if (!node) return TREE_SIMPLIFY_SUCCESS;
    if (TYPE(node) == NUM) return TREE_SIMPLIFY_SUCCESS;
    if (TYPE(node) == VAR) return TREE_SIMPLIFY_SUCCESS;

    TreeSimplifyRes ret_val = TREE_SIMPLIFY_SUCCESS;

    ret_val = SubtreeSimplifyNeutrals(node->left,  tree_changed_flag);
    if (ret_val != TREE_SIMPLIFY_SUCCESS)
        RET_ERROR (TREE_SIMPLIFY_ERR, "Previous function returned error code %d", ret_val);

    ret_val = SubtreeSimplifyNeutrals(node->right, tree_changed_flag);
    if (ret_val != TREE_SIMPLIFY_SUCCESS)
        RET_ERROR (TREE_SIMPLIFY_ERR, "Previous function returned error code %d", ret_val);

    if (TYPE(node) != BI_OP)
        return TREE_SIMPLIFY_SUCCESS; // only binary operators simplification is supported

    switch ((int) VAL(node))
    {
    case ADD:
        if (TYPE(node->left) == NUM && dbleq(VAL(node->left), 0))
        {
            LiftChildToParent (node, RIGHT);

            *tree_changed_flag = 1;
        }
        else if (TYPE(node->right) == NUM && dbleq(VAL(node->right), 0))
        {
            LiftChildToParent (node, LEFT);

            *tree_changed_flag = 1;
        }
        break;

    case SUB:
        if (TYPE(node->right) == NUM && dbleq(VAL(node->right), 0))
        {
            LiftChildToParent (node, LEFT);

            *tree_changed_flag = 1;
        }
        break;

    case MUL:
        if (TYPE(node->left) == NUM && dbleq(VAL(node->left), 1))
        {
            LiftChildToParent (node, RIGHT);

            *tree_changed_flag = 1;
        }
        else if (TYPE(node->right) == NUM && dbleq(VAL(node->right), 1))
        {
            LiftChildToParent (node, LEFT);

            *tree_changed_flag = 1;
        }
        else if (TYPE(node->left) == NUM && dbleq(VAL(node->left), 0))
        {
            SubtreeToNum(node, 0);

            *tree_changed_flag = 1;
        }
        else if (TYPE(node->right) == NUM && dbleq(VAL(node->right), 0))
        {
            SubtreeToNum(node, 0);

            *tree_changed_flag = 1;
        }
        break;

    case POW:
        if (TYPE(node->right) == NUM && dbleq(VAL(node->right), 1))
        {
            LiftChildToParent (node, LEFT);

            *tree_changed_flag = 1;
        }
        else if (TYPE(node->right) && dbleq(VAL(node->right), 0))
        {
            SubtreeToNum(node, 1);

            *tree_changed_flag = 1;
        }
        break;

    default:
        break;
    }

    return ret_val;
}

// ============================================================================================

TreeNode* TreeNodeCtor (double val, NodeType type, TreeNode* prev, TreeNode* left, TreeNode* right)
{
    TreeNode * new_node = (TreeNode *) calloc(1, sizeof(TreeNode));

    new_node->data  = {type, val};
    new_node->prev  = prev;
    new_node->left  = left;
    new_node->right = right;

    return new_node;
}

// ============================================================================================

int TreeNodeDtor (TreeNode * node)
{
    if (node) free(node);

    return 0; // return value in most cases is ignored
}

// ============================================================================================

int SubtreeDtor (TreeNode* node)
{
    assert(node);

    TraverseSubtree(node, TreeNodeDtor, POSTORDER);

    return 0;
}

// ============================================================================================

Tree* TreeCtor ()
{
    Tree* tree = (Tree*) calloc (1, sizeof (Tree));

    NameTableCtor(&tree->nametable);

    return tree;
}

// ============================================================================================

TreeDtorRes TreeDtor (Tree * tree)
{
    assert(tree);
    if (!tree) RET_ERROR (TREE_DTOR_ERR_PARAMS, "Tree null pointer");

    // traverse the tree and free each node
    TraverseTree(tree, TreeNodeDtor, POSTORDER);

    NameTableDtor(&tree->nametable);

    free(tree);

    return TREE_DTOR_SUCCESS;
}

// ============================================================================================

NameTableCtorRes NameTableCtor (NameTable* nametable)
{
    assert(nametable);
    if (!nametable) RET_ERROR(NAMETABLE_CTOR_ERR_PARAMS, "Nametable null pointer");

    for(size_t i = 0; i < NAMETABLE_CAPACITY; i++)
        nametable->names[i] = (char *) calloc(MAX_OP, sizeof(char));

    nametable->free = 0;

    return NAMETABLE_CTOR_SUCCESS;
}

// ============================================================================================

NameTableDtorRes NameTableDtor (NameTable* nametable)
{
    assert(nametable);
    if (!nametable) RET_ERROR(NAMETABLE_DTOR_ERR_PARAMS, "Nametable null pointer");

    for(size_t i = 0; i < NAMETABLE_CAPACITY; i++)
        free(nametable->names[i]);

    nametable->free = 0;


    return NAMETABLE_DTOR_SUCCESS;
}

// ============================================================================================

Tree* TreeCopy (const Tree * tree)
{
    assert (tree);
    if (!tree) RET_ERROR (NULL, "Tree null pointer");

    Tree* copied = TreeCtor ();
    copied->root = SubtreeCopy (tree->root);

    NameTableCopy (&copied->nametable, &tree->nametable);

    copied->size = tree->size;

    return copied;
}

// ============================================================================================

TreeNode * SubtreeCopy (TreeNode * node)
{
    if (!node) return NULL;

    TreeNode * copied = TreeNodeCtor(node->data.val, node->data.type, node->prev,
                                        SubtreeCopy(node->left), SubtreeCopy(node->right));

    return copied;
}

// ============================================================================================

NameTableCopyRes NameTableCopy (NameTable * dst, const NameTable * src)
{
    assert (dst);
    assert (src);
    if (!dst) RET_ERROR (NAMETABLE_COPY_ERR_PARAMS, "Destination nametable null pointer");
    if (!dst) RET_ERROR (NAMETABLE_COPY_ERR_PARAMS, "Source nametable null pointer");

    for (size_t i = 0; i < NAMETABLE_CAPACITY; i++)
    {
        dst->names[i] = strdup(src->names[i]);
        dst->vals[i]  = src->vals[i];
    }
    dst->free = src->free;

    return NAMETABLE_COPY_SUCCESS;
}

// ============================================================================================

LiftChildToParentRes LiftChildToParent (TreeNode* node, NodeLocation child_location)
{
    if (!node) return LIFT_CHILD_TO_PARENT_SUCCESS;

    if (child_location == LEFT)
    {
        if (!node->left)
            RET_ERROR (LIFT_CHILD_TO_PARENT_ERR, "Nothing to lift: left null pointer");

        VAL(node)  = VAL(node->left);
        TYPE(node) = TYPE(node->left);
        TreeNodeDtor(node->right);

        if (node->left->left)  node->left->left->prev  = node;
        if (node->left->right) node->left->right->prev = node;

        node->right = node->left->right;
        TreeNode* old_left = node->left;
        node->left = node->left->left;
        TreeNodeDtor(old_left);

        return LIFT_CHILD_TO_PARENT_SUCCESS;
    }
    if (child_location == RIGHT)
    {
        if (!node->right)
            RET_ERROR (LIFT_CHILD_TO_PARENT_ERR, "Nothing to lift: right null pointer");

        VAL(node)  = VAL(node->right);
        TYPE(node) = TYPE(node->right);
        TreeNodeDtor(node->left);

        if (node->right->left)  node->right->left->prev  = node;
        if (node->right->right) node->right->right->prev = node;

        node->left = node->right->left;
        TreeNode* old_right = node->right;
        node->right = node->right->right;
        TreeNodeDtor(old_right);

        return LIFT_CHILD_TO_PARENT_SUCCESS;
    }

    return LIFT_CHILD_TO_PARENT_ERR;
}

// ============================================================================================

SubtreeToNumRes SubtreeToNum (TreeNode* node, double val)
{
    assert(node);
    if (!node) RET_ERROR(SUBTR_TO_NUM_ERR_PARAMS, "Node null pointer");

    VAL(node)  = val;
    TYPE(node) = NUM;

    SubtreeDtor (node->left);
    SubtreeDtor (node->right);

    node->left  = NULL;
    node->right = NULL;

    return SUBTR_TO_NUM_SUCCESS;
}

// ============================================================================================

TraverseTreeRes TraverseSubtree (TreeNode * node, NodeAction_t NodeAction, TraverseOrder traverse_order)
{
    if (!node) return TRVRS_TREE_SUCCESS;

    if (traverse_order == PREORDER)
    {
        NodeAction (node);
        TraverseSubtree (node->left, NodeAction, traverse_order);
        TraverseSubtree (node->right, NodeAction, traverse_order);
    }
    else if (traverse_order == INORDER)
    {
        TraverseSubtree (node->left, NodeAction, traverse_order);
        NodeAction (node);
        TraverseSubtree (node->right, NodeAction, traverse_order);
    }
    else if (traverse_order == POSTORDER)
    {
        TraverseSubtree (node->left, NodeAction, traverse_order);
        TraverseSubtree (node->right, NodeAction, traverse_order);
        NodeAction (node);
    }
    else
    {
        RET_ERROR (TRVRS_TREE_ERR, "Wrong traversal order: %d\n", traverse_order);
    }

    return TRVRS_TREE_SUCCESS;
}

// ============================================================================================

TraverseTreeRes TraverseTree (Tree * tree, NodeAction_t NodeAction, TraverseOrder traverse_order)
{
    assert (tree);

    return TraverseSubtree(tree->root, NodeAction, traverse_order);
}

// ============================================================================================

TreeNode * SubtreeFind (TreeNode * node, double val, NodeType type)
{
    if (!node)
        return NULL;

    if (node->data.val == val && node->data.type == type)
        return node;

    TreeNode * res = SubtreeFind(node->left, val, type);

    if (res)
        return res;
    else
        return SubtreeFind(node->right, val, type);

}

// ============================================================================================

TreeNode * TreeFind (Tree * tree, double val, NodeType type)
{
    assert(tree);
    assert(val);

    return SubtreeFind (tree->root, val, type);
}

// ============================================================================================

TreeNode* ReadSubtree (const char * infix_tree, NameTable * nametable, int * offset)
{
    assert(infix_tree);

    SkipSpaces(infix_tree, offset);

    if (infix_tree[*offset] == '*')
    {
        *offset += 1;

        return NULL;
    }

    if (infix_tree[*offset] != '(')
    {
        fprintf(stderr, "ReadSubTree: unknown action symbol %c (%d)\n", infix_tree[*offset], infix_tree[*offset]);

        ABORT(); // !
    }

    TreeNode * node = TreeNodeCtor(0, NUM, NULL, NULL, NULL);

    *offset += 1; // skip (

    node->left = ReadSubtree(infix_tree, nametable, offset);
    if (node->left) node->left->prev = node;

    node->data = ReadNodeData(infix_tree, nametable, offset);

    node->right = ReadSubtree(infix_tree, nametable, offset);
    if (node->right) node->right->prev = node;

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
    assert(nametable);
    assert(offset);

    NodeData data = {ERR, 0};

    SkipSpaces(infix_tree, offset);

    char word[MAX_OP] = "";

    int addition = 0;

    sscanf((infix_tree + *offset), "%s%n", word, &addition); // todo what if var name longer than MAX_OP

    *offset += addition;

    if (ReadAssignDouble(&data, word) == READ_ASSIGN_DBL_SUCCESS)
    {
        return data;
    }

    if (ReadAssignOperator(&data, word) == READ_ASSIGN_OP_SUCCESS)
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

WriteTreeRes WriteSubtree(FILE * stream, const TreeNode * node, const NameTable * nametable)
{
    assert(stream);
    if (!stream) RET_ERROR(WRT_TREE_ERR_PARAMS, "Stream null pointer");

    if (!node)
    {
        fprintf(stream, "* ");

        return WRT_TREE_SUCCESS;
    }

    fprintf(stream, "( ");

    WriteSubtree(stream, node->left, nametable);
    WriteTreeRes ret_val = WriteNodeData(stream, node->data, nametable);
    WriteSubtree(stream, node->right, nametable);

    fprintf(stream, ") ");

    return ret_val;
}

// ============================================================================================

WriteTreeRes WriteTree(FILE * stream, const Tree * tree)
{
    assert(stream);
    assert(tree);
    if (!stream) RET_ERROR(WRT_TREE_ERR_PARAMS, "Stream null pointer");
    if (!tree)   RET_ERROR(WRT_TREE_ERR_PARAMS, "Tree null pointer");

    WriteTreeRes ret_val = WriteSubtree(stream, tree->root, (const NameTable *) &tree->nametable);

    fprintf(stream, "\n");

    return ret_val;
}

// ============================================================================================

WriteTreeRes WriteNodeData (FILE * stream, NodeData data, const NameTable * nametable)
{
    assert(stream);
    assert(nametable);
    if (!nametable) RET_ERROR(WRT_TREE_ERR_PARAMS, "Nametable null pointer");
    if (!stream)    RET_ERROR(WRT_TREE_ERR_PARAMS, "Word null pointer");

    int opnum = -1;

    if (data.type == NUM)
    {
        fprintf(stream, "%lf ", data.val);

        return WRT_TREE_SUCCESS;
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

    return WRT_TREE_SUCCESS;
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
        return 1;


    while (*word)
    {
        if (!isalnum(*word) && *word != '_')
            return 1;

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
    assert(data);
    assert(word);
    if (!data) RET_ERROR(READ_ASSIGN_OP_ERR_PARAMS, "data null pointer");
    if (!word) RET_ERROR(READ_ASSIGN_OP_ERR_PARAMS, "word null pointer");

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
    assert(data);
    assert(word);
    if (!data) RET_ERROR(READ_ASSIGN_DBL_ERR_PARAMS, "data null pointer");
    if (!word) RET_ERROR(READ_ASSIGN_DBL_ERR_PARAMS, "word null pointer");

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
    assert(nametable);
    if (!nametable) RET_ERROR(0, "Nametable null pointer");

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
    assert (nametable);
    assert (word);
    if (!nametable) RET_ERROR (ILL_OPNUM, "Nametable null pointer");
    if (!word)      RET_ERROR (ILL_OPNUM, "Word null pointer");

    for (size_t i = 0; i < NAMETABLE_CAPACITY; i++)
    {
        if (streq (nametable->names[i], word))
            return i; // duplicate id
    }

    return -1; // no duplicates
}

int IsDouble (char * word)
{
    // although it handles most used cases, it
    // may be unsafe, not tested properly

    assert (word);
    if (!word) RET_ERROR (0, "Word null pointer");

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

SkipSpacesRes SkipSpaces (const char * string, int* offset)
{
    assert(string);
    assert(offset);
    if (!string) RET_ERROR(SKIP_SPACES_ERR_PARAMS, "string null pointer");
    if (!offset) RET_ERROR(SKIP_SPACES_ERR_PARAMS, "offset null pointer");

    while (isspace(string[*offset]))
        (*offset)++;

    return SKIP_SPACES_SUCCESS;
}
