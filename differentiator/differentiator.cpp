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

    Tree * tree_derivative = TreeCopy(tree);
    TreeNodeDtor(tree_derivative->root);

    tree_derivative->root = TreeNodeCtor(EQUAL, UN_OP, NULL, d(tree->root));

    return tree_derivative;
}

TreeNode * derivative (const TreeNode * node) // ? naming
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
        // return _POW()
        break;

    default:
        break;
    }

    return NULL;
}
