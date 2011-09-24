#ifndef _TEXT_EDITOR_MODEL_H_
#define _TEXT_EDITOR_MODEL_H_

#include <string>

class TextEditorModel {
public:
	virtual ~TextEditorModel() {}

	virtual void clear() = 0;
	virtual void appendLine(std::string line) = 0;
	virtual int getNumLines() = 0;
	virtual std::string getLine(int i) = 0;
	virtual std::string getText() = 0;
	virtual int getCursorLine() = 0;
	virtual int getCursorColumn() = 0;
	virtual void insertText(std::string text) = 0;
	virtual void moveCursor(int dx, int dy) = 0;
	virtual void deleteChar() = 0;
	virtual void backspaceChar() = 0;
	virtual void enter() = 0;
};

#endif // _TEXT_EDITOR_MODEL_H_
