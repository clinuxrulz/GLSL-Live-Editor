#ifndef _TEXT_EDITOR_UI_H_
#define _TEXT_EDITOR_UI_H_

#include "TextEditor.h"

class TextEditorUI {
public:
	virtual ~TextEditorUI() {}

	virtual void scrollToTop() = 0;
	virtual void paint(TextEditor* textEditor) = 0;
	virtual void flashRed() = 0;
	virtual void setErrorText(const char* errorText) = 0;
};

#endif // _TEXT_EDITOR_UI_H_
