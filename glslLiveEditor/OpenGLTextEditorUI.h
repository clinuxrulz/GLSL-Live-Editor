#ifndef _OPENGL_TEXT_EDITOR_UI_H_
#define _OPENGL_TEXT_EDITOR_UI_H_

#include "TextEditorUI.h"
#include <string>

class OpenGLTextEditorUI : public TextEditorUI {
public:
	OpenGLTextEditorUI();

	void scrollToTop();
	void paint(TextEditor* textEditor);
	void flashRed();
	void setErrorText(const char* errorText);

private:
	int numVisibleRows;
	int rowOffset;
	bool redFlash;
	float lastFlashTime;
	std::string errorText;
};

#endif // _OPENGL_TEXT_EDITOR_UI_H_
