#include "glprint.h"
#include "OpenGLTextEditorUI.h"

OpenGLTextEditorUI::OpenGLTextEditorUI() {
	numVisibleRows = 35;
	rowOffset = 0;
	lastFlashTime = 0;
	redFlash = false;
}

void OpenGLTextEditorUI::scrollToTop() {
	rowOffset = 0;
}

bool isBetweenInclusive(int x, int a, int b) {
	return ((a <= x) && (x <= b)) || ((b <= x) && (x <= a));
}

bool isBetweenExclusive(int x, int a, int b) {
	return ((a < x) && (x < b)) || ((b < x) && (x < a));
}

void OpenGLTextEditorUI::paint(TextEditor* textEditor) {
	float t = GetTickCount() * 0.001f;
	float redFactor = 0.0;
	float red, green, blue;
	float selRed, selGreen, selBlue;
	selRed = 0.7f; selGreen = 0.7f; selBlue = 1.0f;
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
		glTranslatef(0.0f, -0.25f, 0.0f);
		glColor3f(0.8f, 0.8f, 1.0f);
		glPrint("%03i:", i+1);
		if (i == textEditor->getCursorLine()) {
			glScalef(1.0f, a, 1.0f);
			red = 0.5f + 0.5f * sin(t * 10.0f); green = 0.8f + 0.2f * sin(t*10.0f); blue = 0.5f + 0.5f * sin(t * 10.0f);
			red *= (1.0f - redFactor); red += redFactor;
			green *= (1.0f - redFactor);
			blue *= (1.0f - redFactor);
			std::string text1 = textEditor->getLine(i).substr(0, textEditor->getCursorColumn());
			std::string text2 = textEditor->getLine(i).substr(textEditor->getCursorColumn());
			if (i == textEditor->getSelectionStartLine() && i == textEditor->getSelectionEndLine()) {
				if (min(textEditor->getSelectionStartColumn(), textEditor->getSelectionEndColumn()) < textEditor->getCursorColumn()) {
					glColor3f(red, green, blue);
					glPrint("%s", text1.substr(0, min(textEditor->getSelectionStartColumn(), textEditor->getSelectionEndColumn())).c_str());
					glColor3f(selRed, selGreen, selBlue);
					glPrint("%s", text1.substr(min(textEditor->getSelectionStartColumn(), textEditor->getSelectionEndColumn())).c_str());
				} else {
					glColor3f(red, green, blue);
					glPrint("%s", text1.c_str());
				}
			} else if (i == textEditor->getSelectionStartLine()) {
				if (textEditor->getSelectionEndLine() > textEditor->getSelectionStartLine()) {
					glColor3f(red, green, blue);
					glPrint("%s", text1.c_str());
				} else {
					glColor3f(selRed, selGreen, selBlue);
					glPrint("%s", text1.c_str());
				}
			} else if (i == textEditor->getSelectionEndLine()) {
				if (textEditor->getSelectionEndLine() > textEditor->getSelectionStartLine()) {
					glColor3f(selRed, selGreen, selBlue);
					glPrint("%s", text1.c_str());
				} else {
					glColor3f(red, green, blue);
					glPrint("%s", text1.c_str());
				}
			} else {
				glColor3f(red, green, blue);
				glPrint("%s", text1.c_str());
			}
			glColor3f(red, green, blue);
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
			if (i == textEditor->getSelectionStartLine() && i == textEditor->getSelectionEndLine()) {
				if (max(textEditor->getSelectionStartColumn(), textEditor->getSelectionEndColumn()) > textEditor->getCursorColumn()) {
					glColor3f(selRed, selGreen, selBlue);
					glPrint("%s", text2.substr(0, max(textEditor->getSelectionStartColumn(), textEditor->getSelectionEndColumn()) - textEditor->getCursorColumn()).c_str());
					glColor3f(red, green, blue);
					glPrint("%s", text2.substr(max(textEditor->getSelectionStartColumn(), textEditor->getSelectionEndColumn()) - textEditor->getCursorColumn()).c_str());
				} else {
					glColor3f(red, green, blue);
					glPrint("%s", text2.c_str());
				}
			} else if (i == textEditor->getSelectionStartLine()) {
				if (textEditor->getSelectionEndLine() > textEditor->getSelectionStartLine()) {
					glColor3f(selRed, selGreen, selBlue);
					glPrint("%s", text2.c_str());
				} else {
					glColor3f(red, green, blue);
					glPrint("%s", text2.c_str());
				}
			} else if (i == textEditor->getSelectionEndLine()) {
				if (textEditor->getSelectionEndLine() > textEditor->getSelectionStartLine()) {
					glColor3f(red, green, blue);
					glPrint("%s", text2.c_str());
				} else {
					glColor3f(selRed, selGreen, selBlue);
					glPrint("%s", text2.c_str());
				}
			} else {
				glColor3f(red, green, blue);
				glPrint("%s", text2.c_str());
			}
		} else {
			red = 0.0f;
			green = 0.8f;
			blue = 0.0f;
			red *= (1.0f - redFactor); red += redFactor;
			green *= (1.0f - redFactor);
			blue *= (1.0f - redFactor);
			if (isBetweenExclusive(i, textEditor->getSelectionStartLine(), textEditor->getSelectionEndLine())) {
				glColor3f(selRed, selGreen, selBlue);
				glPrint("%s", textEditor->getLine(i).c_str());
			} else if (i == textEditor->getSelectionStartLine()) {
				if (textEditor->getSelectionEndLine() > textEditor->getSelectionStartLine()) {
					glColor3f(red, green, blue);
					glPrint("%s", textEditor->getLine(i).substr(0, textEditor->getSelectionStartColumn()).c_str());
					glColor3f(selRed, selGreen, selBlue);
					glPrint("%s", textEditor->getLine(i).substr(textEditor->getSelectionStartColumn()).c_str());
				} else {
					glColor3f(selRed, selGreen, selBlue);
					glPrint("%s", textEditor->getLine(i).substr(0, textEditor->getSelectionStartColumn()).c_str());
					glColor3f(red, green, blue);
					glPrint("%s", textEditor->getLine(i).substr(textEditor->getSelectionStartColumn()).c_str());
				}
			} else if (i == textEditor->getSelectionEndLine()) {
				if (textEditor->getSelectionEndLine() > textEditor->getSelectionStartLine()) {
					glColor3f(selRed, selGreen, selBlue);
					glPrint("%s", textEditor->getLine(i).substr(0, textEditor->getSelectionStartColumn()).c_str());
					glColor3f(red, green, blue);
					glPrint("%s", textEditor->getLine(i).substr(textEditor->getSelectionStartColumn()).c_str());
				} else {
					glColor3f(red, green, blue);
					glPrint("%s", textEditor->getLine(i).substr(0, textEditor->getSelectionStartColumn()).c_str());
					glColor3f(selRed, selGreen, selBlue);
					glPrint("%s", textEditor->getLine(i).substr(textEditor->getSelectionStartColumn()).c_str());
				}
			} else {
				glColor3f(red, green, blue);
				glPrint("%s", textEditor->getLine(i).c_str());
			}
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
