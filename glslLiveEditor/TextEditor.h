#ifndef _TEXT_EDITOR_H_
#define _TEXT_EDITOR_H_

class TextEditorModel;
class TextEditorUI;

#include <string>

class TextEditor {
private:
	TextEditorModel* model;
	TextEditorUI* ui;
	bool keyDown[256];
public:
	TextEditor(TextEditorModel *model, TextEditorUI *ui);

	void keyPressed(int vkey);
	void keyReleased(int vkey);
	void update(float time);
	void repaint();

	int getNumLines();
	std::string getLine(int i);
	int getCursorLine();
	int getCursorColumn();
	int getSelectionStartLine();
	int getSelectionStartColumn();
	int getSelectionEndLine();
	int getSelectionEndColumn();
};

#endif // _TEXT_EDITOR_H_
