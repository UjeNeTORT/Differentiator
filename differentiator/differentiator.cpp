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

#include "differentiator.h"

Tree * derivative (const Tree * tree)
{
    assert(tree);
    if (!tree) RET_ERROR(NULL, "Tree null pointer");

    Tree * tree_derivative = (Tree *) calloc(1, sizeof(Tree));

    tree_derivative->root = TreeNodeCtor(EQUAL, UN_OP, NULL, d(tree->root));
    NameTableCopy (&tree_derivative->nametable, &tree->nametable);

    return tree_derivative;
}

TreeNode * derivative (const TreeNode * node)
{
    if (!node) return NULL;

    if (node->data.type == NUM) return _NUM(0);
    if (node->data.type == VAR) return _NUM(1);

    switch ((int) node->data.val)
    {
    case EQUAL:
        return dR;

    case ADD:
        return _ADD(dL, dR);

    case SUB:
        return _SUB(dL, dR);

    case MUL:
        return _ADD(_MUL(dL, cR), _MUL(cL, dR));

    case DIV:
        return _DIV(_SUB(_MUL(dL, cR), _MUL(cL, dR)), _POW(cR, _NUM(2)));

    case POW:
        return _MUL(_ADD(_MUL(dR, _LN(cL)), _DIV(_MUL(dL, cR), cL)), _POW(cL, cR));

    case EXP:
        return _MUL(dR, _EXP(cR));

    case LN:
        return _MUL(dR, _DIV(_NUM(1), cR));

    case SIN:
        return _MUL(dR, _COS(cR));

    case COS:
        return _MUL(dR, _SUB(_NUM(0), _SIN(cR)));

    case TG:
        return _MUL(dR, _DIV(_NUM(1), _POW(_COS(cR), _NUM(2))));

    case CTG:
        return _MUL(dR, _SUB(_NUM(0), _DIV(_NUM(1), _POW(_SIN(cR), _NUM(2)))));

    case ASIN:
        return _MUL(dR, _DIV(_NUM(1), _POW(_SUB(_NUM(1), _POW(cR, _NUM(2))), _NUM(0.5))));

    case ACOS:
        return _MUL(dR, _SUB(_NUM(0), _DIV(_NUM(1), _POW(_SUB(_NUM(1), _POW(cR, _NUM(2))), _NUM(0.5)))));

    case ATG:
        return _MUL(dR, _DIV(_NUM(1), _ADD(_NUM(1), _POW(cR, _NUM(2)))));

    case ACTG:
        return _MUL(dR, _SUB(_NUM(0), _DIV(_NUM(1), _ADD(_NUM(1), _POW(cR, _NUM(2))))));

    case SH:
        return _MUL(dR, _CH(cR));

    case CH:
        return _MUL(dR, _SH(cR));

    case TH:
        return _MUL(dR, _DIV(_NUM(1), _POW(_CH(cR), _NUM(2))));

    case CTH:
        return _MUL(dR, _SUB(_NUM(0), _DIV(_NUM(1), _POW(_SH(cR), _NUM(2)))));

    default:
        RET_ERROR(NULL, "Operation %d not found\n", (int) node->data.val);
        break;
    }

    return NULL;
}
