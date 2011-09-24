#include "TextEditor.h"
#include "TextEditorModel.h"
#include "TextEditorUI.h"

TextEditor::TextEditor(TextEditorModel *model, TextEditorUI *ui) {
	this->model = model;
	this->ui = ui;
}

void TextEditor::keyPressed(int vkey) {
}

void TextEditor::keyReleased(int vkey) {
}

void TextEditor::update(float time) {
	repaint();
}

void TextEditor::repaint() {
	ui->paint(this);
}

int TextEditor::getNumLines() {
	return model->getNumLines();
}

std::string TextEditor::getLine(int i) {
	return model->getLine(i);
}

int TextEditor::getCursorLine() {
	return model->getCursorLine();
}

static int min(int a, int b) {
	if (a < b) {
		return a;
	} else {
		return b;
	}
}

static int max(int a, int b) {
	if (a > b) {
		return a;
	} else {
		return b;
	}
}

int TextEditor::getCursorColumn() {
	return min(model->getCursorColumn(), getLine(getCursorLine()).length());
}
