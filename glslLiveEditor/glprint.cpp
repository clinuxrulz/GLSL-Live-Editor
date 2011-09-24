#include "glprint.h"

static int base;
static GLYPHMETRICSFLOAT gmf[256];

// http://msdn.microsoft.com/en-us/library/dd183327(v=vs.85).aspx
// AddFontResourceEx function
// for private fonts from files

GLvoid buildFont(HDC hDC) {
	HFONT font;
	HFONT oldFont;

	AddFontResourceEx("Anonymous Pro.ttf", FR_PRIVATE, 0);

	base = glGenLists(256);

	font = CreateFont(-24, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, FF_DONTCARE|DEFAULT_PITCH, "Anonymous Pro");
	oldFont = (HFONT)SelectObject(hDC, font);
	wglUseFontOutlines(hDC, 0, 255, base, 0.0f, /*extrusion=*/0.0f, WGL_FONT_POLYGONS, gmf);
	SelectObject(hDC, oldFont);
	DeleteObject(font);
}

GLvoid killFont(GLvoid) {
	glDeleteLists(base, 256);
}

GLvoid glPrint(const char *fmt, ...) {
	char text[256];
	va_list ap;
	if (fmt == NULL) { return; }
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);
	glPushAttrib(GL_LIST_BIT);
	glListBase(base);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}

GLvoid glPrintCentredX(const char *fmt, ...) {
	char text[256];
	va_list ap;
	if (fmt == NULL) { return; }
	va_start(ap, fmt);
	vsprintf_s(text, fmt, ap);
	va_end(ap);
	float length=0.0;
	for (int i = 0; i < (int)strlen(text); ++i) {
		length += gmf[text[i]].gmfCellIncX;
	}
	glTranslatef(-length * 0.5f, 0.0f, 0.0f);
	glPushAttrib(GL_LIST_BIT);
	glListBase(base);
	glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);
	glPopAttrib();
}
