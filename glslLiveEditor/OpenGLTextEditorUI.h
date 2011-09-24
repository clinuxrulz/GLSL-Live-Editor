#ifndef _OPENGL_TEXT_EDITOR_UI_H_
#define _OPENGL_TEXT_EDITOR_UI_H_

#include "TextEditorUI.h"

class OpenGLTextEditor : public TextEditorUI {
public:
	OpenGLTextEditor();

	void scrollToTop();
	void paint(TextEditor* textEditor);

private:
	int numVisibleRows;
	int rowOffset;
};

#endif // _OPENGL_TEXT_EDITOR_UI_H_
