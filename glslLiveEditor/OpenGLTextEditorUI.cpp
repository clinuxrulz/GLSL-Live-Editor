#include "glprint.h"
#include "OpenGLTextEditorUI.h"

OpenGLTextEditorUI::OpenGLTextEditorUI() {
	numVisibleRows = 28;
	rowOffset = 0;
	lastFlashTime = 0;
	redFlash = false;
}

void OpenGLTextEditorUI::scrollToTop() {
	rowOffset = 0;
}

void OpenGLTextEditorUI::paint(TextEditor* textEditor) {
	float t = GetTickCount() * 0.001f;
	float redFactor = 0.0;
	float red, green, blue;
	if (redFlash) {
		if (t - lastFlashTime >= 1.0f) {
			redFlash = false;
		} else {
			redFactor = 1.0f - (t - lastFlashTime);
		}
	}
	if (textEditor->getCursorLine() >= rowOffset + numVisibleRows) {
		rowOffset = textEditor->getCursorLine() - numVisibleRows + 1;
	} else if (textEditor->getCursorLine() < rowOffset) {
		rowOffset = textEditor->getCursorLine();
	}
	float a = 1.0f + 0.1f * sin(t * 20.0f);
	float b = sin(t * 2.5f) * 0.25f - 0.25f;
	glPushMatrix();
	glScalef(0.7f, 0.7f, 0.7f);
	for (int i = max(0, rowOffset); i < min(textEditor->getNumLines(), rowOffset + numVisibleRows); ++i) {
		glPushMatrix();
		if (i == textEditor->getCursorLine()) {
			glScalef(1.0f, a, 1.0f);
			glTranslatef(0.0f, -0.25f, 0.0f);
			red = 0.5f + 0.5f * sin(t * 10.0f); green = 0.8f + 0.2f * sin(t*10.0f); blue = 0.5f + 0.5f * sin(t * 10.0f);
			red *= (1.0f - redFactor); red += redFactor;
			green *= (1.0f - redFactor);
			blue *= (1.0f - redFactor);
			glColor3f(red, green, blue);
			std::string text1 = textEditor->getLine(i).substr(0, textEditor->getCursorColumn());
			std::string text2 = textEditor->getLine(i).substr(textEditor->getCursorColumn());
			glPrint("%s", text1.c_str());
			glScalef(1.0f, 1.0f / a, 1.0f);
			glBegin(GL_QUADS);
			glVertex3f(0.1f, -0.2f, 0.1f);
			glVertex3f(0.2f, -0.2f, 0.1f);
			glVertex3f(0.2f, +0.6f, 0.1f);
			glVertex3f(0.1f, +0.6f, 0.1f);
			//glColor3f(0.5f + 0.5f * sin(t * 10.0f), 0.8f + 0.2f * sin(t*10.0f), 0.5f + 0.5f * sin(t * 10.0f));
			glVertex3f(0.15f, b + 0.1f, 0.1f);
			glVertex3f(0.05f, b, 0.1f);
			glVertex3f(0.15f, b - 0.1f, 0.1f);
			glVertex3f(0.25f, b, 0.1f);
			glEnd();
			glScalef(1.0f, a, 1.0f);
			glPrint("%s", text2.c_str());
		} else {
			glTranslatef(0.0f, -0.25f, 0.0f);
			red = 0.0f;
			green = 0.8f;
			blue = 0.0f;
			red *= (1.0f - redFactor); red += redFactor;
			green *= (1.0f - redFactor);
			blue *= (1.0f - redFactor);
			glColor3f(red, green, blue);
			glPrint("%s", textEditor->getLine(i).c_str());
		}
		glPopMatrix();
		glTranslatef(0.0f, -1.2f, 0.0f);
	}
	glTranslatef(0.0f, -0.25f, 0.0f);
	if (!errorText.empty()) {
		glColor3f(1.0f, 1.0f, 1.0f);
		glPrint(errorText.c_str());
	}
	glPopMatrix();
}

void OpenGLTextEditorUI::flashRed() {
	redFlash = true;
	lastFlashTime = GetTickCount() * 0.001f;
}

void OpenGLTextEditorUI::setErrorText(const char* errorText) {
	this->errorText = errorText;
}
