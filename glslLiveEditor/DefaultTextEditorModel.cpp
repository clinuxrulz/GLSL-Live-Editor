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
	std::string text2 = text;
	bool newLine = false;
	if (text2[text2.length()-1] == '\n') {
		text2 = text2.substr(0, text2.length()-1);
		newLine = true;
	}
	cursorColumn = min(cursorColumn, lines[cursorLine].length());
	lines[cursorLine].insert(cursorColumn, text2);
	if (newLine) {
		cursorColumn = 0;
		lines.insert(lines.begin() + cursorLine + 1, 1, std::string(""));
		++cursorLine;
	}
}

void DefaultTextEditorModel::moveCursor(int dx, int dy) {
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
}

void DefaultTextEditorModel::deleteChar() {
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
