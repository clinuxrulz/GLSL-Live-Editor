#ifndef _DEFAULT_TEXT_EDITOR_MODEL_H_
#define _DEFAULT_TEXT_EDITOR_MODEL_H_

#include <vector>
#include <string>
#include "TextEditorModel.h"

class DefaultTextEditorModel : public TextEditorModel {
public:
	DefaultTextEditorModel();

	virtual void clear();
	virtual void appendLine(std::string line);
	virtual int getNumLines();
	virtual std::string getLine(int i);
	virtual std::string getText();
	virtual int getCursorLine();
	virtual int getCursorColumn();
	virtual void insertText(std::string text);
	virtual void moveCursor(int dx, int dy);
	virtual void deleteChar();
	virtual void backspaceChar();
	virtual void enter();
private:
	int cursorLine;
	int cursorColumn;
	std::vector<std::string> lines;
};

#endif // _DEFAULT_TEXT_EDITOR_MODEL_H_
