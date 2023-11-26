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

int TreeDump (const char * HTML_fname, const Tree * tree)
{
    assert (HTML_fname);

    srand(time(0));
    int dump_id = rand();

    char dot_fname[100] = "";
    sprintf(dot_fname, "graph_%d.dot", dump_id);
    DotTreePrint (dot_fname, tree);

    char detailed_dot_fname[100] = "";
    sprintf(detailed_dot_fname, "detailed_graph_%d.dot", dump_id);
    DotTreeDetailedPrint (detailed_dot_fname, tree);

    char * command = (char *) calloc (COMMAND_BUF_SIZE, sizeof(char));
    sprintf (command, "dot -Tpng %s%s -o %sgraph_dump_%d.png", DOT_FILE_PATH, dot_fname, GRAPH_PNGS_PATH, dump_id);
    system (command);

    sprintf (command, "dot -Tpng %s%s -o %sdetailed_graph_dump_%d.png", DOT_FILE_PATH, detailed_dot_fname, GRAPH_PNGS_PATH, dump_id);
    system (command);

    free(command);

    WriteHTML(HTML_fname, dump_id);

    return dump_id;
}

int DotTreePrint (const char * dot_fname, const Tree * tree)
{
    assert (dot_fname);

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

    fprintf (HTML_file, "<img src=\"../../../../%sgraph_dump_%d.png\">\n", GRAPH_PNGS_PATH, dump_id); //! ../../...crutch - dont know how to specify relative path

    fprintf(HTML_file, "</div>\n");
    /////////////////////////////////
    fprintf(HTML_file, "<div detailed_graph_%d style=\"background-color: %s; color: %s;\">\n", dump_id, GRAPH_BGCLR, GRAPH_TEXTCLR);

    fprintf (HTML_file, "<p style=\"color: %s; font-family:monospace; font-size: 20px\">[%s] TREE DETAILED </p>", "#283D3B", asctime(loc_time));

    fprintf (HTML_file, "<img src=\"../../../../%sdetailed_graph_dump_%d.png\">\n", GRAPH_PNGS_PATH, dump_id); //! ../../...crutch - dont know how to specify relative path

    fprintf(HTML_file, "</div>\n");

    fprintf (HTML_file, "<hr> <!-- ============================================================================================================================ --> <hr>\n");
    fprintf (HTML_file, "\n");

    fclose (HTML_file);

    free(HTML_path);

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

    // todo function
    const char * color = "";
    char node_data[MAX_OP] = "";

    switch (node->data.type)
    {
    case NUM:
        color = GRAPH_NUMCLR;
        sprintf(node_data, "%.2lf", node->data.val);
        break;

    case VAR: // ! doestn work
        color = GRAPH_VARCLR;
        sprintf(node_data, "%s", nametable.table[(int) node->data.val]); // get varname from nametable
        break;

    case BI_OP:
        color = GRAPH_OPCLR;
        //! copypaste
        switch ((int) node->data.val)
        {
            case ADD:
                sprintf(node_data, "+");
                break;

            case SUB:
                sprintf(node_data, "-");
                break;

            case MUL:
                sprintf(node_data, "*");
                break;

            case DIV:
                sprintf(node_data, "/");
                break;

            case POW:
                sprintf(node_data, "^");
                break;

            default:
                fprintf(stdout, "Err wrong opcode\n");
                break;
        }
        break;

    case UN_OP: // ! doesnt work
        color = GRAPH_OPCLR;

        switch ((int) node->data.val)
        {
            case EQUAL:
                sprintf(node_data, "=");
                break;


            default:
                break;
        }
        break;

    default:
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
