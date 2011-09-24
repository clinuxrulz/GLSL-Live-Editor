#ifndef _GL_PRINT_H_
#define _GL_PRINT_H_

#include <Windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <gl/GL.h>
#include <gl/GLU.h>

GLvoid buildFont(HDC hDC);
GLvoid killFont(GLvoid);
GLvoid glPrint(const char *fmt, ...);

#endif // _GL_PRINT_H_
