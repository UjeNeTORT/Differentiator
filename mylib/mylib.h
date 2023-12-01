/*************************************************************************
 * (c) 2023 Tikhonov Yaroslav (aka UjeNeTORT)
 *
 * email: tikhonovty@gmail.com
 * telegram: https://t.me/netortofficial
 * GitHub:   https://github.com/UjeNeTORT
 * repo:     https://github.com/UjeNeTORT/Tree
 *************************************************************************/

#ifndef MYLIB_H
#define MYLIB_H

#include <stdio.h>
#include <stdlib.h>

#define streq(s1, s2) (!strcmp((s1), (s2)))

// todo ask ded how to not destroy PC
#define ABORT()                                             \
{                                                           \
    fprintf(stderr, "ABORT ABORT ABORT - MAKE FUNCTION\n"); \
}                                                           \

#define PRINTF_DEBUG(format, ...) \
    PrintfDebug (__FUNCTION__, __LINE__, __FILE__, format __VA_OPT__(,) __VA_ARGS__)

#define WARN(format, ...) \
    PrintfWarning (__FUNCTION__, __LINE__, __FILE__, format __VA_OPT__(,) __VA_ARGS__)

#define ERROR(format, ...) \
    PrintfError (__FUNCTION__, __LINE__, __FILE__, format __VA_OPT__(,) __VA_ARGS__)

#define RET_ERROR(err_code, format, ...) \
    { PrintfError (__FUNCTION__, __LINE__, __FILE__, format __VA_OPT__(,) __VA_ARGS__); \
      return err_code;}

int PrintfDebug   (const char * funcname, int line, const char * filename, const char * format, ...); // todo COPYPASTE, have to write unique function for all of theese
int PrintfDebug   (const char * funcname, int line, const char * filename, const char * format, ...); // todo COPYPASTE, have to write unique function for all of theese
int PrintfWarning (const char * funcname, int line, const char * filename, const char * format, ...); // todo COPYPASTE, have to write unique function for all of theese



#endif // MYLIB_H
