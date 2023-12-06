/*************************************************************************
 * (c) 2023 Tikhonov Yaroslav (aka UjeNeTORT)
 *
 * email: tikhonovty@gmail.com
 * telegram: https://t.me/netortofficial
 * GitHub:   https://github.com/UjeNeTORT
 * repo:     https://github.com/UjeNeTORT/Differentiator
 *************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include "differentiator.h"

Tree* Derivative (const Tree * tree)
{
    assert (tree);
    if (!tree) RET_ERROR (NULL, "Tree null pointer");

    Tree* tree_derivative = (Tree*) calloc(1, sizeof(Tree));

    NameTableCopy (&tree_derivative->nametable, &tree->nametable);

    tree_derivative->root = TreeNodeCtor (EQUAL, UN_OP, NULL, NULL, Derivative (tree->root));

    return tree_derivative;
}

TreeNode* Derivative (const TreeNode* node)
{
    assert(node);

    return Derivative(NULL, node, NULL);
}

TreeNode* Derivative (FILE* tex_file, const TreeNode* node, const NameTable* nametable)
{
    if (!node) return NULL;

    if (node->data.type == ERR) RET_ERROR (NULL, "Error node passed to differentiation function\n");

    if (node->data.type == NUM) return _NUM(0);
    if (node->data.type == VAR) return _NUM(1);

    TreeNode* d_node = NULL;

    switch ((int) node->data.val)
    {
    case EQUAL:
        d_node = dR;
        break;

    case ADD:
        d_node = _ADD(dL, dR);
        break;

    case SUB:
        d_node = _SUB(dL, dR);
        break;

    case MUL:
        d_node = _ADD(_MUL(dL, cR), _MUL(cL, dR));
        break;

    case DIV:
        d_node = _DIV(_SUB(_MUL(dL, cR), _MUL(cL, dR)), _POW(cR, _NUM(2)));
        break;

    case POW:
        d_node = _MUL(_ADD(_MUL(dR, _LN(cL)), _DIV(_MUL(dL, cR), cL)), _POW(cL, cR));
        break;

    case EXP:
        d_node = _MUL(dR, _EXP(cR));
        break;

    case LN:
        d_node = _MUL(dR, _DIV(_NUM(1), cR));
        break;

    case SIN:
        d_node = _MUL(dR, _COS(cR));
        break;

    case COS:
        d_node = _MUL(dR, _SUB(_NUM(0), _SIN(cR)));
        break;

    case TG:
        d_node = _MUL(dR, _DIV(_NUM(1), _POW(_COS(cR), _NUM(2))));
        break;

    case CTG:
        d_node = _MUL(dR, _SUB(_NUM(0), _DIV(_NUM(1), _POW(_SIN(cR), _NUM(2)))));
        break;

    case ASIN:
        d_node = _MUL(dR, _DIV(_NUM(1), _POW(_SUB(_NUM(1), _POW(cR, _NUM(2))), _NUM(0.5))));
        break;

    case ACOS:
        d_node = _MUL(dR, _SUB(_NUM(0), _DIV(_NUM(1), _POW(_SUB(_NUM(1), _POW(cR, _NUM(2))), _NUM(0.5)))));
        break;

    case ATG:
        d_node = _MUL(dR, _DIV(_NUM(1), _ADD(_NUM(1), _POW(cR, _NUM(2)))));
        break;

    case ACTG:
        d_node = _MUL(dR, _SUB(_NUM(0), _DIV(_NUM(1), _ADD(_NUM(1), _POW(cR, _NUM(2))))));
        break;

    case SH:
        d_node = _MUL(dR, _CH(cR));
        break;

    case CH:
        d_node = _MUL(dR, _SH(cR));
        break;

    case TH:
        d_node = _MUL(dR, _DIV(_NUM(1), _POW(_CH(cR), _NUM(2))));
        break;

    case CTH:
        d_node = _MUL(dR, _SUB(_NUM(0), _DIV(_NUM(1), _POW(_SH(cR), _NUM(2)))));
        break;

    default:
        RET_ERROR(NULL, "Operation %d not found\n", (int) node->data.val);
        break;
    }

    // todo treesimplify

    if (tex_file && (int) node->data.val != EQUAL)
    {
        fprintf(tex_file, "%s\n", MATAN_PHRASES[rand() % MATAN_PHRASES_COUNT]);
        fprintf(tex_file, "$$  (");
        TexSubtreePrint(tex_file, NULL, node, nametable);
        fprintf(tex_file, ")\' = $$\n");

        fprintf(tex_file, "$$  ");
        TexSubtreePrint(tex_file, node, d_node, nametable);
        fprintf(tex_file, "  $$\n");
    }
    return d_node;
}

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
        if (TreeEvalNums (node, &VAL(node)) != TREE_EVAL_SUCCESS) return TREE_SIMPLIFY_ERR;
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
        if (TreeEvalNums (node, &VAL(node)) != TREE_EVAL_SUCCESS) return TREE_SIMPLIFY_ERR;
        TYPE(node) = NUM;

        TreeNodeDtor(node->left);

        node->left = NULL;

        *tree_changed_flag = 1;

        return ret_val;
    }

    return ret_val;
}

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

Tree* DerivativeReport (const Tree* tree)
{
    assert(tree);

    Tree* diff_tree = TreeCtor();
    NameTableCopy(&diff_tree->nametable, &tree->nametable);

    char tex_path[MAX_PATH] = "";
    FILE* tex_file = InitTexDump(diff_tree, tex_path);

    diff_tree->root = TreeNodeCtor (EQUAL, UN_OP, NULL, NULL,
                                    SubtreeDerivativeTexReport(tex_file, tree->root,
                                                               &diff_tree->nametable));

    ConcludeTexDump(tex_file, tex_path);

    CompileLatex(tex_path);

    return diff_tree;
}

TreeNode* SubtreeDerivativeTexReport (FILE* tex_file, const TreeNode* node, const NameTable* nametable)
{
    if (!tex_file) RET_ERROR(NULL, "Tex stream null pointer");

    return Derivative(tex_file, node, nametable);
}
