/*************************************************************************
 * (c) 2023 Tikhonov Yaroslav (aka UjeNeTORT)
 *
 * email: tikhonovty@gmail.com
 * telegram: https://t.me/netortofficial
 * GitHub:   https://github.com/UjeNeTORT
 * repo:     https://github.com/UjeNeTORT/Differentiator
 *************************************************************************/

#ifndef DIFFERENTIATOR_H
#define DIFFERENTIATOR_H

#include "../tree/tree.h"
#include "../tree/tree_dump/tree_dump.h"

// ===================== DSL =====================

#define d(node_or_tree) derivative(node_or_tree)
#define c(node)         SubtreeCopy(node)

#define dL d(node->left)
#define dR d(node->right)
#define cL c(node->left)
#define cR c(node->right)

#define _NUM(num)         TreeNodeCtor(num, NUM,   NULL, NULL)
#define _ADD(LEFT, RIGHT) TreeNodeCtor(ADD, BI_OP, LEFT, RIGHT)
#define _SUB(LEFT, RIGHT) TreeNodeCtor(SUB, BI_OP, LEFT, RIGHT)
#define _MUL(LEFT, RIGHT) TreeNodeCtor(MUL, BI_OP, LEFT, RIGHT)
#define _DIV(LEFT, RIGHT) TreeNodeCtor(DIV, BI_OP, LEFT, RIGHT)
#define _POW(LEFT, RIGHT) TreeNodeCtor(POW, BI_OP, LEFT, RIGHT)

#define _EXP(RIGHT)       TreeNodeCtor(EXP, UN_OP, NULL, RIGHT)
#define _LN(RIGHT)        TreeNodeCtor(LN,  UN_OP, NULL, RIGHT)
#define _SIN(RIGHT)       TreeNodeCtor(SIN, UN_OP, NULL, RIGHT)
#define _COS(RIGHT)       TreeNodeCtor(COS, UN_OP, NULL, RIGHT)
#define _SH(RIGHT)        TreeNodeCtor(SH,  UN_OP, NULL, RIGHT)
#define _CH(RIGHT)        TreeNodeCtor(CH,  UN_OP, NULL, RIGHT)

// ===============================================

Tree     * derivative (const Tree * tree);
TreeNode * derivative (const TreeNode * node);

#endif // DIFFERENTIATOR_H
