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
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "tree_dump.h"

int TreeTexDump (const char * PDF_fname, const Tree * tree)
{
    assert(PDF_fname);
    assert(tree);

    srand(time(0));

    int dump_id = (int) time(NULL);

    char tex_fname[MAX_TREE_FNAME] = "";
    sprintf(tex_fname, "expr_%d.tex", dump_id);

    TexTreePrint(tex_fname, tree);

    char * tex_path = GetFilePath(TEX_FILE_PATH, tex_fname);
    char command[COMMAND_BUF_SIZE] = "";
    sprintf(command, "pdflatex -output-directory=%s %s ", PDF_DUMPS_PATH, tex_path);
    system(command);

    sprintf(command, "	rm -f %s/*.aux"
	                 "  rm -f %s/*.log", PDF_DUMPS_PATH, PDF_DUMPS_PATH);
    system(command);

    free(tex_path);

    return dump_id;
}

int TreeDotDump (const char * HTML_fname, const Tree * tree)
{
    assert (HTML_fname);
    assert(tree);

    srand(time(0));

    int dump_id = (int) time(NULL);

    char dot_fname[MAX_TREE_FNAME] = "";
    sprintf(dot_fname, "graph_%d.dot", dump_id);
    DotTreePrint (dot_fname, tree);

    char detailed_dot_fname[MAX_TREE_FNAME] = "";
    sprintf(detailed_dot_fname, "detailed_graph_%d.dot", dump_id);
    DotTreeDetailedPrint (detailed_dot_fname, tree);

    char command[COMMAND_BUF_SIZE] = "";
    sprintf (command, "dot -Tsvg %s%s -o %sgraph_dump_%d.svg", DOT_FILE_PATH, dot_fname, GRAPH_SVGS_PATH, dump_id);
    system (command);

    sprintf (command, "dot -Tsvg %s%s -o %sdetailed_graph_dump_%d.svg", DOT_FILE_PATH, detailed_dot_fname, GRAPH_SVGS_PATH, dump_id);
    system (command);

    WriteHTML(HTML_fname, dump_id);

    return dump_id;
}

int TexTreePrint (const char * tex_fname, const Tree * tree)
{
    assert(tex_fname);
    assert(tree);

    char * tex_path = GetFilePath(TEX_FILE_PATH, tex_fname);

    FILE * tex_file = fopen(tex_path, "wb");

    time_t t = time(NULL);
    tm * loc_time = localtime(&t);

    fprintf(tex_file,  "\\documentclass[a4paper,12pt]{article}\n\n"
                       "\\usepackage{amsmath}\n"
                       "\\DeclareMathOperator\\arcctan{arcctan}\n"
                       "\\title{EXPRESSION DUMP}\n"
                       "\\author{Tikhonov Yaroslav (aka UjeNeTORT)}\n"
                       "\\date{Date: %d.%d.%d, Time %d:%d:%d}\n"
                       "\\begin{document}\n"
                       "\\maketitle\n",
                        loc_time->tm_mday, loc_time->tm_mon + 1, loc_time->tm_year + 1900,
                        loc_time->tm_hour, loc_time->tm_min, loc_time->tm_sec);

    fprintf(tex_file, "$$   ");

    TexSubtreePrint (tex_file, tree->root, tree->root->right, tree->nametable);

    fprintf(tex_file, "$$    \n");

    fprintf(tex_file, "\\end{document}\n");
    fclose(tex_file);

    free(tex_path);

    return 0;
}

int DotTreePrint (const char * dot_fname, const Tree * tree)
{
    assert(dot_fname);
    assert(tree);

    char * dot_path = GetFilePath(DOT_FILE_PATH, dot_fname);

    FILE * dot_file = fopen (dot_path, "wb");

    fprintf (dot_file, "digraph TREE {\n"
                        "bgcolor =\"%s\"", GRAPH_BGCLR);

    DotSubtreePrint (dot_file, tree->root, tree->nametable);

    fprintf (dot_file, "}\n");

    fclose (dot_file);

    free(dot_path);

    return 0;
}

int WriteHTML (const char * HTML_fname, int dump_id)
{
    assert (HTML_fname);

    char * HTML_path = GetFilePath(HTML_DUMPS_PATH, HTML_fname);

    time_t t = time (NULL);

    tm * loc_time = localtime (&t);

    FILE * HTML_file = fopen (HTML_path, "wb"); // ! attention, deletion of old dumps
    fprintf(HTML_file, "<div graph_%d style=\"background-color: %s; color: %s;\">\n", dump_id, GRAPH_BGCLR, GRAPH_TEXTCLR);

    fprintf (HTML_file, "<p style=\"color: %s; font-family:monospace; font-size: 20px\">[%s] TREE</p>", "#283D3B", asctime(loc_time));

    fprintf (HTML_file, "<img src=\"../../../../%sgraph_dump_%d.svg\">\n", GRAPH_SVGS_PATH, dump_id); //! ../../...crutch - dont know how to specify relative path

    fprintf(HTML_file, "</div>\n");

    fprintf(HTML_file, "<div detailed_graph_%d style=\"background-color: %s; color: %s;\">\n", dump_id, GRAPH_BGCLR, GRAPH_TEXTCLR);

    fprintf (HTML_file, "<p style=\"color: %s; font-family:monospace; font-size: 20px\">[%s] TREE DETAILED </p>", "#283D3B", asctime(loc_time));

    fprintf (HTML_file, "<img src=\"../../../../%sdetailed_graph_dump_%d.svg\">\n", GRAPH_SVGS_PATH, dump_id); //! ../../...crutch - dont know how to specify relative path

    fprintf(HTML_file, "</div>\n");

    fprintf (HTML_file, "<hr> <!-- ============================================================================================================================ --> <hr>\n");
    fprintf (HTML_file, "\n");

    fclose (HTML_file);

    free(HTML_path);

    return 0;
}

int TexSubtreePrint (FILE * stream, const TreeNode * prev, const TreeNode * node, NameTable nametable)
{
    assert(stream);

    if (!node)
    {
        return 0;
    }

    const char * op_name = NULL;

    int opnum = 0;
    int print_parenthesis = 0;

    switch(node->data.type)
    {
        case ERR:
            fprintf(stderr, "ERROR! Tex Dump Printer mustnt have entered ERR nodes!\n");
            return 1;
            break;

        case NUM:
            fprintf(stream, " {%.3lf} ", node->data.val);
            break;

        case VAR:
            fprintf(stream, " {%s} ", nametable.names[(int) node->data.val]);
            break;

        case UN_OP:
            opnum = FindOperation((int) node->data.val);
            op_name = OPERATIONS[opnum].name;

            if (!streq(op_name, "="))
            {
                fprintf(stream, " \\%s ", op_name);
                TexSubtreePrint(stream, node, node->right, nametable);
            }
            else
            {
                TexSubtreePrint(stream, node, node->right, nametable);
            }

            break;

        case BI_OP:
            if ((opnum = FindOperation((int) node->data.val)) == ILL_OPNUM)
            {
                fprintf(stderr, "TexSubtreePrint: no such operation (%d)\n", (int) node->data.val);
                abort();
            }

            if (FindOperation((int) prev->data.val) > FindOperation((int) node->data.val)) //! here we assume that prev is operation node
            {
                print_parenthesis = 1;
            }

            if (print_parenthesis)
                fprintf(stream, " ( ");

            fprintf(stream, " { ");
            TexSubtreePrint(stream, node, node->left, nametable);
            fprintf(stream, " } ");

            fprintf(stream, " %s ", OPERATIONS[opnum].name);

            fprintf(stream, " { ");
            TexSubtreePrint(stream, node, node->right, nametable);
            fprintf(stream, " } ");

            if (print_parenthesis)
                fprintf(stream, " ) ");

            break;

        default:
            PRINTF_DEBUG("default case");
            break;
    }

    return 0;
}

int DotSubtreePrint (FILE * stream, const TreeNode * node, NameTable nametable)
{
    assert (stream);

    if (!node)
    {
        return 0;
    }

    int node_id = rand();

    const char * color = "";
    char node_data[MAX_OP] = "";

    int opnum = 0;

    switch (node->data.type)
    {
    case NUM:
        color = GRAPH_NUMCLR;
        sprintf(node_data, "%.2lf", node->data.val);
        break;

    case VAR:
        color = GRAPH_VARCLR;
        sprintf(node_data, "%s", nametable.names[(int) node->data.val]); // get varname from nametable
        break;

    case BI_OP:
        color = GRAPH_OPCLR;

        opnum = FindOperation((int) node->data.val);
        sprintf(node_data, "%s", OPERATIONS[opnum].name);

        break;

    case UN_OP: // ! doesnt work
        color = GRAPH_OPCLR;

        opnum = FindOperation((int) node->data.val);
        sprintf(node_data, "%s", OPERATIONS[opnum].name);
        break;
    }

    fprintf (stream, "\tnode_%d [style = filled, shape = circle, label = \"%s\", fillcolor = \"%s\", fontcolor = \"%s\"];\n", node_id, node_data, color, GRAPH_TEXTCLR);

    if (int left_subtree_id = DotSubtreePrint (stream, node->left, nametable))
        fprintf (stream, "\tnode_%d -> node_%d;\n", node_id, left_subtree_id);

    if (int right_subtree_id = DotSubtreePrint (stream, node->right, nametable))
        fprintf (stream, "\tnode_%d -> node_%d;\n", node_id, right_subtree_id);

    return node_id;
}

int DotTreeDetailedPrint (const char * dot_fname, const Tree * tree)
{
    assert(dot_fname);

    char * dot_path = GetFilePath(DOT_FILE_PATH, dot_fname);
    FILE * dot_file = fopen(dot_path, "wb");

    fprintf (dot_file, "digraph DETAILED_TREE {\n"
                        "bgcolor =\"%s\"", GRAPH_BGCLR);

    DotSubtreeDetailedPrint(dot_file, (const TreeNode *) tree->root, tree->nametable);

    fprintf (dot_file, "}");

    fclose(dot_file);
    free(dot_path);

    return 0;
}

int DotSubtreeDetailedPrint (FILE * stream, const TreeNode * node, NameTable nametable)
{
    assert(stream);

    if (!node)
    {
        return 0;
    }

    int node_id = rand();

    const char * color = "";

    switch (node->data.type)
    {
    case NUM:
        color = GRAPH_NUMCLR;
        break;

    case VAR:
        color = GRAPH_VARCLR;
        break;

    case BI_OP:
        color = GRAPH_OPCLR;
        break;

    case UN_OP:
        color = GRAPH_OPCLR;
        break;

    default:
        break;
    }

    fprintf (stream, "\tdetailed_node_%d [style = filled, shape = record, fillcolor = \"%s\", fontcolor = \"%s\"];\n", node_id, color, GRAPH_TEXTCLR);
    fprintf (stream, "\tdetailed_node_%d [label = \"{type = %d | val = %.3lf}\"];\n", node_id, node->data.type, node->data.val);

    if (int left_subtree_id = DotSubtreeDetailedPrint (stream, (const TreeNode *) node->left, nametable))
        fprintf (stream, "\tdetailed_node_%d -> detailed_node_%d;\n", node_id, left_subtree_id);

    if (int right_subtree_id = DotSubtreeDetailedPrint (stream, (const TreeNode *) node->right, nametable))
        fprintf (stream, "\tdetailed_node_%d -> detailed_node_%d;\n", node_id, right_subtree_id);

    return node_id;
}

char * GetFilePath(const char * path, const char * fname)
{
    char * fpath = (char *) calloc(MAX_PATH, sizeof(char));

    strcat(fpath, path); // path must have / in the end
    strcat(fpath, fname);

    return fpath;
}
