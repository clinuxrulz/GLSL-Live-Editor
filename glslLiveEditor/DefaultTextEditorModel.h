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
	virtual void shiftDown();
	virtual void shiftUp();
	virtual bool isTextSelected();
	virtual int getSelectionStartLine();
	virtual int getSelectionEndLine();
	virtual int getSelectionStartColumn();
	virtual int getSelectionEndColumn();
	virtual std::string getSelectedText();
	virtual void setSelectedText(std::string text);
private:
	bool shiftIsDown;
	int cursorLine;
	int cursorColumn;
	int selectionStartRow;
	int selectionStartColumn;
	int selectionEndRow;
	int selectionEndColumn;
	std::vector<std::string> lines;
};

#endif // _DEFAULT_TEXT_EDITOR_MODEL_H_
