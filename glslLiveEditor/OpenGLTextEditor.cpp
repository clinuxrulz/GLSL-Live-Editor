#include "glprint.h"
#include "OpenGLTextEditorUI.h"

OpenGLTextEditor::OpenGLTextEditor() {
	numVisibleRows = 40;
	rowOffset = 0;
}

void OpenGLTextEditor::scrollToTop() {
	rowOffset = 0;
}

void OpenGLTextEditor::paint(TextEditor* textEditor) {
	if (textEditor->getCursorLine() >= rowOffset + numVisibleRows) {
		rowOffset = textEditor->getCursorLine() - numVisibleRows + 1;
	} else if (textEditor->getCursorLine() < rowOffset) {
		rowOffset = textEditor->getCursorLine();
	}
	float t = GetTickCount() / 1000.0f;
	float a = 1.0f + 0.1f * sin(t * 20.0f);
	float b = sin(t * 2.5f) * 0.25f - 0.25f;
	glPushMatrix();
	glScalef(0.7f, 0.7f, 0.7f);
	for (int i = max(0, rowOffset); i < min(textEditor->getNumLines(), rowOffset + numVisibleRows); ++i) {
		glPushMatrix();
		if (i == textEditor->getCursorLine()) {
			glScalef(1.0f, a, 1.0f);
			glTranslatef(0.0f, -0.25f, 0.0f);
			glColor3f(0.5f + 0.5f * sin(t * 10.0f), 0.8f + 0.2f * sin(t*10.0f), 0.5f + 0.5f * sin(t * 10.0f));
			std::string text1 = textEditor->getLine(i).substr(0, textEditor->getCursorColumn());
			std::string text2 = textEditor->getLine(i).substr(textEditor->getCursorColumn());
			glPrint("%s", text1.c_str());
			glScalef(1.0f, 1.0f / a, 1.0f);
			glBegin(GL_QUADS);
			glVertex3f(0.1f, -0.2f, 0.1f);
			glVertex3f(0.2f, -0.2f, 0.1f);
			glVertex3f(0.2f, +0.6f, 0.1f);
			glVertex3f(0.1f, +0.6f, 0.1f);
			glColor3f(0.5f + 0.5f * sin(t * 10.0f), 0.8f + 0.2f * sin(t*10.0f), 0.5f + 0.5f * sin(t * 10.0f));
			glVertex3f(0.15f, b + 0.1f, 0.1f);
			glVertex3f(0.05f, b, 0.1f);
			glVertex3f(0.15f, b - 0.1f, 0.1f);
			glVertex3f(0.25f, b, 0.1f);
			glEnd();
			glScalef(1.0f, a, 1.0f);
			glPrint("%s", text2.c_str());
		} else {
			glTranslatef(0.0f, -0.25f, 0.0f);
			glColor3f(0.0f, 0.8f, 0.0f);
			glPrint("%s", textEditor->getLine(i).c_str());
		}
		glPopMatrix();
		glTranslatef(0.0, -1.2, 0.0);
	}
	glPopMatrix();
}
