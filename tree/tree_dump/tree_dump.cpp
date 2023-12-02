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

FILE* InitTexDump (const Tree* tree, char * tex_path)
{
    assert (tree);
    assert (tex_path);

    srand(time(0));
    int dump_id = (int) time(NULL);

    sprintf(tex_path, "expr_%d.tex", dump_id); // stores filename
    char* temp_tex_path = GetFilePath (TEX_FILE_PATH, tex_path);
    strcpy(tex_path, temp_tex_path);
    free(temp_tex_path);

    FILE * tex_file = fopen (tex_path, "wb");

    time_t t = time (NULL);
    tm * loc_time = localtime (&t);

    fprintf (tex_file,  "\\documentclass[a4paper,12pt]{article}\n\n"
                        "\\usepackage{amsmath}\n"
                        "\\DeclareMathOperator\\arcctan{arcctan}\n"
                        "\\title{EXPRESSION DUMP}\n"
                        "\\author{Tikhonov Yaroslav (aka UjeNeTORT)}\n"
                        "\\date{Date: %d.%d.%d, Time %d:%d:%d}\n"
                        "\\begin{document}\n"
                        "\\maketitle\n",
                        loc_time->tm_mday, loc_time->tm_mon + 1, loc_time->tm_year + 1900,
                        loc_time->tm_hour, loc_time->tm_min, loc_time->tm_sec);

    return tex_file;
}

int TreeTexDump (const Tree * tree)
{
    assert(tree);

    char tex_path[MAX_TREE_FNAME] = "";

    FILE* tex_file = InitTexDump(tree, tex_path);

    if (TexTreePrint(tex_file, tree) != TEX_PRINT_SUCCESS)
    {
        RET_ERROR(-1, "Tree wasnt printed");
    }

    ConcludeTexDump(tex_file, tex_path);

    CompileLatex(tex_path);

    return 0;
}

int CompileLatex (char * tex_path)
{
    assert(tex_path);

    CompileTex(tex_path);

    return 0;
}

int CompileTex(const char* tex_path)
{
    assert(tex_path);

    char command[COMMAND_BUF_SIZE] = "";

    sprintf(command, "pdflatex -output-directory=%s %s", PDF_DUMPS_PATH, tex_path);
    system(command);

    sprintf(command, "	rm -f %s/*.aux"
	                 "  rm -f %s/*.log", PDF_DUMPS_PATH, PDF_DUMPS_PATH);
    system(command);

    return 0;
}

int ConcludeTexDump (FILE* tex_file, char * tex_path)
{
    assert(tex_file);
    assert(tex_path);

    fprintf (tex_file, "\\end{document}\n");
    fclose (tex_file);

    return 0;
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

    CompileDot(dot_fname, detailed_dot_fname, dump_id);

    WriteHTML(HTML_fname, dump_id);

    return dump_id;
}

int CompileDot (char* dot_fname, char* detailed_dot_fname, int dump_id)
{
    assert(dot_fname);
    assert(detailed_dot_fname);

    char command[COMMAND_BUF_SIZE] = "";
    sprintf (command, "dot -Tsvg %s%s -o %sgraph_dump_%d.svg", DOT_FILE_PATH, dot_fname, GRAPH_SVGS_PATH, dump_id);
    system (command);

    sprintf (command, "dot -Tsvg %s%s -o %sdetailed_graph_dump_%d.svg", DOT_FILE_PATH, detailed_dot_fname, GRAPH_SVGS_PATH, dump_id);
    system (command);

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

TexTreePrintRes TexTreePrint (FILE* tex_file, const Tree * tree)
{
    assert (tex_file);
    assert (tree);

    if (!tex_file) RET_ERROR(TEX_PRINT_ERR, "Tex file null pointer");
    if (!tree)     RET_ERROR(TEX_PRINT_ERR, "Tree null pointer");

    fprintf(tex_file, "$$  ");

    if (tree->root)
        TexSubtreePrint (tex_file, tree->root, tree->root->right, &tree->nametable);
    else
    {
        // it is not in the beginning because we may still need other info about the tree
        // apart from tree being shown itself

        RET_ERROR (TEX_PRINT_ERR, "Tree root null pointer");
    }

    fprintf(tex_file, "  $$\n");

    return TEX_PRINT_SUCCESS;
}

DotTreePrintRes DotTreePrint (const char * dot_fname, const Tree * tree)
{
    assert(dot_fname);
    assert(tree);
    if (!dot_fname) RET_ERROR(DOT_PRINT_ERR, "Tex filename null pointer\n");
    if (!tree)      RET_ERROR(DOT_PRINT_ERR, "Tree null pointer\n");

    char * dot_path = GetFilePath(DOT_FILE_PATH, dot_fname);

    FILE * dot_file = fopen (dot_path, "wb");

    fprintf (dot_file, "digraph TREE {\n"
                        "bgcolor =\"%s\"", GRAPH_BGCLR);

    DotSubtreePrint (dot_file, tree->root, tree->nametable);

    fprintf (dot_file, "}\n");

    fclose (dot_file);

    free(dot_path);

    return DOT_PRINT_SUCCESS;
}

TexSubtreePrintRes TexSubtreePrint (FILE * tex_file, const TreeNode * prev, const TreeNode * node, const NameTable* nametable)
{
    assert(tex_file);

    if (!prev) WARN("Prev null pointer (braces wont be printed)\n");

    if (!node)
    {
        return TEX_SUBT_PRINT_SUCCESS;
    }

    const char * op_name = NULL;

    int opnum = 0;
    int print_parenthesis = 0;

    switch(node->data.type)
    {
        case ERR:
            RET_ERROR(TEX_SUBT_PRINT_ERR, "Printer received error node");

        case NUM:
            fprintf(tex_file, " {%.3lf} ", node->data.val);
            break;

        case VAR:
            fprintf(tex_file, " {%s} ", nametable->names[(int) node->data.val]);
            break;

        case UN_OP:
            opnum = FindOperation((int) node->data.val);
            op_name = OPERATIONS[opnum].name;

            if (!streq(op_name, "="))
            {
                fprintf(tex_file, " \\%s ", op_name);
                TexSubtreePrint(tex_file, node, node->right, nametable);
            }
            else
            {
                TexSubtreePrint(tex_file, node, node->right, nametable);
            }

            break;

        case BI_OP:
            if ((opnum = FindOperation((int) node->data.val)) == ILL_OPNUM)
            {
                RET_ERROR(TEX_SUBT_PRINT_ERR, "No such operation (%d)\n", (int) node->data.val);
            }

            if (prev && OPERATIONS[FindOperation((int) prev->data.val)].priority >
                        OPERATIONS[FindOperation((int) node->data.val)].priority) //! here we assume that prev is operation node
            {
                    print_parenthesis = 1;
            }

            if (print_parenthesis)
                fprintf(tex_file, " ( ");

            if (OPERATIONS[opnum].opcode == DIV)
                fprintf(tex_file, "\\frac");

            fprintf(tex_file, " { ");
            TexSubtreePrint(tex_file, node, node->left, nametable);
            fprintf(tex_file, " } ");

            if (OPERATIONS[opnum].opcode != DIV)
                fprintf(tex_file, " %s ", OPERATIONS[opnum].name);

            fprintf(tex_file, " { ");
            TexSubtreePrint(tex_file, node, node->right, nametable);
            fprintf(tex_file, " } ");

            if (print_parenthesis)
                fprintf(tex_file, " ) ");

            break;

        default:
            RET_ERROR(TEX_SUBT_PRINT_ERR, "Invalid node type \"%d\"\n", node->data.type);
    }

    return TEX_SUBT_PRINT_SUCCESS;
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
    case ERR:
        color = GRAPH_ERRCLR;
        sprintf(node_data, "ERR");
        break;

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
