/*************************************************************************
 * (c) 2023 Tikhonov Yaroslav (aka UjeNeTORT)
 *
 * email: tikhonovty@gmail.com
 * telegram: https://t.me/netortofficial
 * GitHub:   https://github.com/UjeNeTORT
 * repo:     https://github.com/UjeNeTORT/Differentiator
 *************************************************************************/

#ifndef TREE_DUMP_H
#define TREE_DUMP_H

#include <stdio.h>

#include "../tree.h"

const char DOT_FILE_PATH[]   = "tree/tree_dump/dumps/dot_files/";
const char GRAPH_SVGS_PATH[] = "tree/tree_dump/dumps/png/";
const char HTML_DUMPS_PATH[] = "tree/tree_dump/dumps/dumps/";
const char TEX_FILE_PATH[]   = "tree/tree_dump/dumps/tex/";
const char PDF_DUMPS_PATH[]  = "tree/tree_dump/dumps/pdf";

const int DOT_CODE_BUF_SIZE = 15000;

const int COMMAND_BUF_SIZE = 400;
const int MAX_PATH = 100;
const int MAX_TREE_FNAME = 100;

const char GRAPH_BGCLR[]   = "#EDDDD4";
const char GRAPH_TEXTCLR[] = "#EDDDD4";
const char GRAPH_NUMCLR[]  = "#197278";
const char GRAPH_OPCLR[]   = "#C44536";
const char GRAPH_VARCLR[]  = "#283D3B";

typedef enum
{
    TEX_PRINT_SUCCESS = 0,
    TEX_PRINT_ERR     = 1,
} TexTreePrintRes;

typedef enum
{
    DOT_PRINT_SUCCESS = 0,
    DOT_PRINT_ERR     = 1,
} DotTreePrintRes;

int TreeTexDump     (const Tree * tree);
int TreeDotDump     (const char * fname, const Tree * tree);

TexTreePrintRes TexTreePrint    (const char * tex_fname, const Tree * tree);
DotTreePrintRes DotTreePrint    (const char * dot_fname, const Tree * tree);

int WriteHTML       (const char * HTML_fname, int dump_id);

int TexSubtreePrint (FILE * stream, const TreeNode * prev, const TreeNode * node, NameTable nametable);
int DotSubtreePrint (FILE * stream, const TreeNode * node, NameTable nametable);

int DotTreeDetailedPrint    (const char * dot_fname, const Tree * tree);
int DotSubtreeDetailedPrint (FILE * stream, const TreeNode * node, NameTable nametable);

char * GetFilePath (const char * path, const char * fname);

#endif // TREE_DUMP_H
