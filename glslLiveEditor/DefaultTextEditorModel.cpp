#include "DefaultTextEditorModel.h"
#include <sstream>

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

static int clamp(int x, int a, int b) {
	if (x < a) {
		return a;
	} else if (x > b) {
		return b;
	} else {
		return x;
	}
}

DefaultTextEditorModel::DefaultTextEditorModel() {
	cursorLine = 0;
	cursorColumn = 0;
	selectionStartRow = -1;
	selectionStartColumn = -1;
	selectionEndRow = -1;
	selectionEndColumn = -1;
	shiftIsDown = false;
	lines.resize(1);
}

void DefaultTextEditorModel::clear() {
	cursorLine = 0;
	cursorColumn = 0;
	lines.clear();
	lines.push_back(std::string());
}

void DefaultTextEditorModel::appendLine(std::string line) {
	lines.push_back(line);
}

int DefaultTextEditorModel::getNumLines() {
	return lines.size();
}

std::string DefaultTextEditorModel::getLine(int i) {
	return lines[i];
}

std::string DefaultTextEditorModel::getText() {
	std::ostringstream os;
	for (int i = 0; i < (int)lines.size(); ++i) {
		os << lines[i] << std::endl;
	}
	return os.str();
}

int DefaultTextEditorModel::getCursorLine() {
	return cursorLine;
}

int DefaultTextEditorModel::getCursorColumn() {
	return cursorColumn;
}

void DefaultTextEditorModel::insertText(std::string text) {
	if (isTextSelected()) { setSelectedText(text); return; }
	if (cursorColumn > lines[cursorLine].length()) { cursorColumn = lines[cursorLine].length(); }
	lines[cursorLine] = lines[cursorLine].substr(0, cursorColumn) + text + lines[cursorLine].substr(cursorColumn);
	expandNewLines(cursorLine);
	for (int i = 0; i < text.length(); ++i) {
		moveCursor(1, 0);
	}
	selectionStartRow = -1;
	selectionStartColumn = -1;
	selectionEndRow = -1;
	selectionEndColumn = -1;
}

void DefaultTextEditorModel::moveCursor(int dx, int dy) {
	int lastCursorColumn = cursorColumn;
	int lastCursorLine = cursorLine;
	cursorColumn += dx;
	cursorLine = clamp(cursorLine + dy, 0, lines.size()-1);
	if (cursorColumn < 0) {
		if (cursorLine > 0) {
			cursorLine = max(cursorLine-1, 0);
			cursorColumn = lines[cursorLine].length();
		} else {
			cursorColumn = 0;
		}
	}
	if (dx != 0 && cursorColumn > (int)lines[cursorLine].length()) {
		cursorLine = min(cursorLine + 1, lines.size()-1);
		cursorColumn = 0;
	}
	if (shiftIsDown) {
		if (!isTextSelected()) {
			selectionStartRow = lastCursorLine;
			selectionStartColumn = lastCursorColumn;
		}
		selectionEndRow = cursorLine;
		selectionEndColumn = cursorColumn;
	} else {
		selectionStartRow = -1;
		selectionStartColumn = -1;
		selectionEndRow = -1;
		selectionEndColumn = -1;
	}
}

void DefaultTextEditorModel::deleteChar() {
	if (isTextSelected()) { setSelectedText(""); return; }
	int columnMax = lines[cursorLine].length();
	if (cursorColumn < columnMax) {
		lines[cursorLine] = lines[cursorLine].substr(0, cursorColumn) + lines[cursorLine].substr(cursorColumn + 1);
	} else {
		if (cursorLine < (int)lines.size()-1) {
			lines[cursorLine] = lines[cursorLine] + lines[cursorLine + 1];
			lines.erase(lines.begin() + cursorLine + 1);
		}
	}
}

void DefaultTextEditorModel::backspaceChar() {
	if (isTextSelected()) { setSelectedText(""); return; }
	int columnMax = lines[cursorLine].length();
	cursorColumn = min(cursorColumn, columnMax);
	if (cursorColumn > 0) {
		lines[cursorLine] = lines[cursorLine].substr(0, cursorColumn-1) + lines[cursorLine].substr(cursorColumn);
		moveCursor(-1, 0);
	} else {
		if (cursorLine > 0) {
			cursorColumn = lines[cursorLine-1].length();
			lines[cursorLine-1] = lines[cursorLine-1] + lines[cursorLine];
			lines.erase(lines.begin() + cursorLine);
			--cursorLine;
		}
	}
}

void DefaultTextEditorModel::enter() {
	if (isTextSelected()) { setSelectedText("\n"); return; }
	cursorColumn = min(cursorColumn, lines[cursorLine].length());
	std::string newLine = lines[cursorLine].substr(cursorColumn);
	lines[cursorLine] = lines[cursorLine].substr(0, cursorColumn);
	++cursorLine;
	cursorColumn = 0;
	lines.insert(lines.begin() + cursorLine, 1, newLine);
	int pad = 0;
	while (pad < (int)lines[cursorLine-1].length() && lines[cursorLine-1][pad] == ' ') { ++pad; }
	lines[cursorLine] = std::string(pad, ' ') + lines[cursorLine];
	moveCursor(pad, 0);
}

bool DefaultTextEditorModel::isShiftDown() {
	return shiftIsDown;
}

void DefaultTextEditorModel::shiftDown() {
	shiftIsDown = true;
}

void DefaultTextEditorModel::shiftUp() {
	shiftIsDown = false;
}

bool DefaultTextEditorModel::isTextSelected() {
	return selectionStartRow != -1;
}

int DefaultTextEditorModel::getSelectionStartLine() {
	return selectionStartRow;
}

int DefaultTextEditorModel::getSelectionEndLine() {
	return selectionEndRow;
}

int DefaultTextEditorModel::getSelectionStartColumn() {
	return selectionStartColumn;
}

int DefaultTextEditorModel::getSelectionEndColumn() {
	return selectionEndColumn;
}

std::string DefaultTextEditorModel::getSelectedText() {
	if (!isTextSelected()) { return ""; }
	int firstRow;
	int lastRow;
	int firstCol;
	int lastCol;
	if (selectionStartRow < selectionEndRow) {
		firstRow = selectionStartRow;
		firstCol = selectionStartColumn;
		lastRow = selectionEndRow;
		lastCol = selectionEndColumn;
	} else {
		firstRow = selectionEndRow;
		firstCol = selectionEndColumn;
		lastRow = selectionStartRow;
		lastCol = selectionStartColumn;
	}
	if (firstRow == lastRow) {
		if (firstCol > lastCol) {
			int tmp = firstCol;
			firstCol = lastCol;
			lastCol = tmp;
		}
		return lines[firstRow].substr(firstCol, lastCol - firstCol + 1);
	} else {
		std::string text = lines[firstRow].substr(firstCol);
		text += "\n";
		for (int i = firstRow+1; i < lastRow; ++i) {
			text += lines[i];
			text += "\n";
		}
		text += lines[lastRow].substr(0, lastCol);
		return text;
	}
}

void DefaultTextEditorModel::setSelectedText(std::string text) {
	if (!isTextSelected()) { return; }
	int firstRow;
	int lastRow;
	int firstCol;
	int lastCol;
	if (selectionStartRow < selectionEndRow) {
		firstRow = selectionStartRow;
		firstCol = selectionStartColumn;
		lastRow = selectionEndRow;
		lastCol = selectionEndColumn;
	} else {
		firstRow = selectionEndRow;
		firstCol = selectionEndColumn;
		lastRow = selectionStartRow;
		lastCol = selectionStartColumn;
	}
	if (firstRow == lastRow) {
		if (firstCol > lastCol) {
			int tmp = firstCol;
			firstCol = lastCol;
			lastCol = tmp;
		}
		lines[firstRow] = lines[firstRow].substr(0, firstCol) + text + lines[firstRow].substr(lastCol);
		expandNewLines(firstRow);
		selectionStartRow = -1;
		selectionStartColumn = -1;
		selectionEndRow = -1;
		selectionEndColumn = -1;
		cursorLine = firstRow;
		cursorColumn = firstCol;
		if (text.length() > 0) {
			for (int i = 0; i < text.length(); ++i) {
				moveCursor(1, 0);
			}
		}
	} else {
		lines[firstRow] = lines[firstRow].substr(0, firstCol) + text;
		lines[lastRow] = lines[lastRow].substr(lastCol);
		lines.erase(lines.begin() + firstRow, lines.begin() + lastRow);
		expandNewLines(firstRow);
		selectionStartRow = -1;
		selectionStartColumn = -1;
		selectionEndRow = -1;
		selectionEndColumn = -1;
		cursorLine = firstRow;
		cursorColumn = firstCol;
		if (text.length() > 0) {
			for (int i = 0; i < text.length(); ++i) {
				moveCursor(1, 0);
			}
		}
	}
}

void DefaultTextEditorModel::expandNewLines(int row) {
	for (int i = 0; i < lines[row].length(); ++i) {
		if (lines[row][i] == '\n') {
			lines.insert(lines.begin() + row + 1, 1, lines[row].substr(i+1));
			lines[row] = lines[row].substr(0, i);
			++row;
			i = 0;
		}
	}
}
