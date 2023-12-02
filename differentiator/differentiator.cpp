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

    tree_derivative->root = TreeNodeCtor (EQUAL, UN_OP, NULL, Derivative (tree->root));

    return tree_derivative;
}

Tree* DerivativeReport (FILE* tex_stream, const Tree * tree)
{
    assert(tree);
    if (!tree) RET_ERROR (NULL, "Tree null pointer");

    char tex_fname[MAX_TREE_FNAME] = "";

    FILE* tree_file = InitTexDump(tree, tex_fname);

    Tree* tree_derivative = (Tree*) calloc(1, sizeof(Tree));

    NameTableCopy (&tree_derivative->nametable, &tree->nametable);

    // tree_derivative->root = TreeNodeCtor (EQUAL, UN_OP, NULL,
                                    // DerivativeReport (tex_stream, tree->root, &tree_derivative->nametable));

    return tree_derivative;
}

TreeNode* Derivative (const TreeNode* node)
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

    return d_node;
}

int DerivativeTexReport (FILE* tex_stream, const TreeNode* d_node, const TreeNode* node, const NameTable* nametable)
{
    if (!tex_stream) RET_ERROR(1, "Tex stream null pointer");
    if (!nametable)  RET_ERROR(1, "Nametable null pointer");

    // derivative of
    TexSubtreePrint(tex_stream, NULL, node, nametable);
    // is equal
    TexSubtreePrint(tex_stream, NULL, d_node, nametable);

    return 0;
}
