/*************************************************************************
 * (c) 2023 Tikhonov Yaroslav (aka UjeNeTORT)
 *
 * email: tikhonovty@gmail.com
 * telegram: https://t.me/netortofficial
 * GitHub:   https://github.com/UjeNeTORT
 * repo:     https://github.com/UjeNeTORT/Differentiator
 *************************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "differentiator.h"

// ============================================================================================

Tree* Derivative (const Tree* tree)
{
    assert (tree);
    if (!tree) RET_ERROR (NULL, "Tree null pointer");

    Tree* diff_tree = TreeCtor();

    NameTableCopy (diff_tree->nametable, tree->nametable);

    diff_tree->root = TreeNodeCtor (EQUAL, UN_OP, NULL, NULL,
                                                Derivative (tree->root, diff_tree));

    // if (TreeSimplify (diff_tree) != TREE_SIMPLIFY_SUCCESS)
                // RET_ERROR(NULL, "Tree simplification function returned error code");

    return diff_tree;
}

// ============================================================================================

TreeNode* Derivative (const TreeNode* node, Tree* tree)
{
    assert(node);

    return Derivative (node, tree, NULL);
}

// ============================================================================================

TreeNode* Derivative (const TreeNode* node, Tree* tree, FILE* tex_file)
{
    if (tex_file && !tree) RET_ERROR(NULL, "No tree pointer, check docs");

    if (!node) return NULL;

    if (TYPE(node) == ERR) RET_ERROR (NULL, "Error node passed to differentiation function\n");

    if (TYPE(node) == NUM) return _NUM(0);

    if (TYPE(node) == VAR)
    {
        if (tree->nametable->dx_id == (int) VAL(node))
            return _NUM(1);
        else
            return _NUM(0);
    }

    TreeNode* d_node = NULL;

    switch ((int) VAL(node))
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
        // 2^2
        if (!SubtreeHasVars(node->left, tree->nametable) &&
            !SubtreeHasVars(node->right, tree->nametable))
        {
            d_node = _NUM(0);
        }
        // x^2
        else if (SubtreeHasVars(node->left, tree->nametable) &&
                !SubtreeHasVars(node->right, tree->nametable))
        {
            d_node = _MUL(dL, _MUL(cR, _POW(cL, _SUB(cR, _NUM(1)))));
        }
        // 2^x
        else if (!SubtreeHasVars(node->left, tree->nametable) &&
                            SubtreeHasVars(node->right, tree->nametable))
        {
            d_node = _MUL(dR, _MUL(_POW(cL, cR), _LN(cL)));
        }
        // x^x
        else
        {
            d_node = _MUL(_ADD(_MUL(dR, _LN(cL)), _DIV(_MUL(dL, cR), cL)), _POW(cL, cR));
        }

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
        RET_ERROR(NULL, "Operation %d not found\n", (int) VAL(node));
        break;
    }

    int dummy_tree_changed_flag = 0;

    if (SubtreeSimplifyConstants(d_node, &dummy_tree_changed_flag) != TREE_SIMPLIFY_SUCCESS)
        RET_ERROR(NULL, "Tree constants simplification error. Tree corrupted.");

    if (SubtreeSimplifyNeutrals (d_node, &dummy_tree_changed_flag) != TREE_SIMPLIFY_SUCCESS)
        RET_ERROR(NULL, "Tree neutrals simplification error. Tree corrupted.");

    if (tex_file && (int) VAL(node) != EQUAL)
    {
        fprintf(tex_file, "%s\n", MATAN_PHRASES[rand() % MATAN_PHRASES_COUNT]);
        fprintf(tex_file, "$$  (");
        TexSubtreePrint(tex_file, NULL, node, tree);
        fprintf(tex_file, ")_{d %s}\' = $$\n", tree->nametable->names[tree->nametable->dx_id]);

        fprintf(tex_file, "$$ = ");
        TexSubtreePrint(tex_file, node, d_node, tree);
        fprintf(tex_file, "  $$\n");
    }

    return d_node;
}

// ============================================================================================

Tree* DerivativeReport (const Tree* tree)
{
    assert(tree);

    Tree* diff_tree = TreeCtor();
    NameTableCopy(diff_tree->nametable, tree->nametable);

    char tex_path[MAX_PATH] = "";
    FILE* tex_file = InitTexDump(diff_tree, tex_path);

    diff_tree->root = TreeNodeCtor (EQUAL, UN_OP, NULL, NULL,
                                        Derivative(tree->root, diff_tree, tex_file));

    ConcludeTexDump(tex_file);

    CompileLatex(tex_path);

    return diff_tree;
}


