#include <Windows.h>
#include <gl/GL.h>
#include <math.h>
#include <fstream>
#include "glext.h"
#include "net_radio.h"
#include "glprint.h"
#include "TextEditor.h"
#include "DefaultTextEditorModel.h"
#include "OpenGLTextEditorUI.h"

// Minimal TechnoStream
// http://www.subflow.net:8000/listen.pls

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")

static HDC hDC = NULL;

static TextEditorModel* textEditorModel;
static TextEditorUI* textEditorUI;
static TextEditor* textEditor;

static int shaderProgram  = -1;
static int vertexShader   = -1;
static int fragmentShader = -1;

static bool showEditor = true;

void initGl() {
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	buildFont(hDC);
	textEditorModel = new DefaultTextEditorModel();
	textEditorUI = new OpenGLTextEditor();
	textEditor = new TextEditor(textEditorModel, textEditorUI);
}

void initStartingCode() {
	textEditorModel->insertText("#ifdef GL_ES\n");
	textEditorModel->insertText("precision highp float;\n");
	textEditorModel->insertText("#endif\n");
	textEditorModel->insertText("\n");
	textEditorModel->insertText("uniform vec3 unResolution;\n");
	textEditorModel->insertText("uniform float time;\n");
	textEditorModel->insertText("uniform sampler2D tex0;\n");
	textEditorModel->insertText("uniform sampler2D tex1;\n");
	textEditorModel->insertText("uniform sampler2D fft;\n");
	textEditorModel->insertText("uniform vec4 unPar;\n");
	textEditorModel->insertText("uniform vec4 unPos;\n");
	textEditorModel->insertText("uniform vec3 unBeatBassFFT;\n");
	textEditorModel->insertText("\n");
	textEditorModel->insertText("void main() {\n");
	textEditorModel->insertText("  vec3 col = vec3(0.0);\n");
	textEditorModel->insertText("  \n");
	textEditorModel->insertText("  gl_FragColor = vec4(col, 1.0);\n");
	textEditorModel->insertText("}\n");
	textEditorModel->moveCursor(+2 - textEditorModel->getCursorColumn(), +15 - textEditorModel->getCursorLine());
}

PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLDETACHSHADERPROC glDetachShader = NULL;
PFNGLDELETESHADERPROC glDeleteShader = NULL;
PFNGLDELETEPROGRAMPROC glDeleteProgram = NULL;
PFNGLGETSHADERIVPROC glGetShaderiv = NULL;
PFNGLGETPROGRAMIVPROC glGetProgramiv = NULL;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = NULL;
PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = NULL;
PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = NULL;

static void initShaders() {
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)wglGetProcAddress("glGetProgramInfoLog");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
}

static void finalShaders() {
	if (shaderProgram != -1) {
		glDetachShader(shaderProgram, vertexShader);
		glDetachShader(shaderProgram, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderProgram);
		vertexShader = -1;
		fragmentShader = -1;
		shaderProgram = -1;
	}
}

void reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(75.0f, (float)w / (float)h, 100.0, 100000.0);
	glMatrixMode(GL_MODELVIEW);
}

void finalGl() {
	killFont();
	delete textEditor;
	delete textEditorModel;
	delete textEditorUI;
}

void setGLSLUniforms() {
	int time = glGetUniformLocation(shaderProgram, "time");
	glUniform1f(time, GetTickCount() / 1000.0f);
}

void render() {
	if (shaderProgram != -1) {
		glUseProgram(shaderProgram);
		setGLSLUniforms();
		glColor3f(1.0f,0.0f,0.0f);
		glLoadIdentity();
		glBegin(GL_QUADS);
		glVertex3f(-300.0f, -300.0f, -210.0f);
		glVertex3f(+300.0f, -300.0f, -210.0f);
		glVertex3f(+300.0f, +300.0f, -210.0f);
		glVertex3f(-300.0f, +300.0f, -210.0f);
		glEnd();
	}
	if (showEditor) {
		glUseProgram(0);
		glLoadIdentity();
		glColor4f(0.0f,0.0f,0.0f,0.9f);
		glBegin(GL_QUADS);
		glVertex3f(-270.0f, -125.0f, -205.0f);
		glVertex3f(+50.0f, -125.0f, -205.0f);
		glVertex3f(+50.0f, +125.0f, -205.0f);
		glVertex3f(-270.0f, +125.0f, -205.0f);
		glEnd();
		glTranslatef(-250.0f,110.0f,-200.0f);
		glScalef(10.0f, 10.0f, 10.0f);
		textEditor->repaint();
	}
}

void loadShader() {
	std::string line;
	std::ifstream is("fragment_shader.glsl");
	if (!is) {
		return;
	}
	textEditorModel->clear();
	if (std::getline(is, line)) {
		textEditorModel->insertText(line);
		while (std::getline(is, line)) {
			textEditorModel->appendLine(line);
		}
	}
	is.close();
}

void saveShader() {
	std::ofstream os("fragment_shader.glsl");
	for (int i = 0; i < textEditorModel->getNumLines(); ++i) {
		os << textEditorModel->getLine(i) << std::endl;
	}
	os.close();
}

// vertex shader
const GLchar *vsh="\
void main()\
{\
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
}";

void compileShader() {
	glUseProgram(0);
	std::string _fsh = textEditorModel->getText();
	const GLchar *fsh = (GLchar*)_fsh.c_str();
	int is_compiled, is_linked;
	if (vertexShader == -1) {
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vsh, 0);
		glCompileShader(vertexShader);
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &is_compiled);
		if (is_compiled == FALSE) {
			// TODO: set text editor to flash red once
			glDeleteShader(vertexShader);
			vertexShader = -1;
			return;
		}
	}
	if (fragmentShader != -1) {
		glDetachShader(shaderProgram, fragmentShader);
		glDeleteShader(fragmentShader);
	}
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsh, 0);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &is_compiled);
	if (is_compiled == FALSE) {
		// TODO: set text editor to flash red once
		glDeleteShader(fragmentShader);
		fragmentShader = -1;
		return;
	}
	if (shaderProgram != -1) {
		glDeleteProgram(shaderProgram);
	}
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &is_linked);
	if (is_linked == FALSE) {
		// TODO: set text editor to flash red once
		glDetachShader(shaderProgram, vertexShader);
		glDetachShader(shaderProgram, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		glDeleteProgram(shaderProgram);
		vertexShader = -1;
		fragmentShader = -1;
		shaderProgram = -1;
		return;
	}
	//vertexShader = glCreateShaderObject
}

PIXELFORMATDESCRIPTOR pfd={0, 0, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // wrong way ;)

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CHAR: {
		char c = (char)wParam;
		if (c >= 32 && c <= 128) {
			textEditorModel->insertText(std::string() + c);
			textEditorModel->moveCursor(+1, +0);
		} else if (c == '\t') {
			textEditorModel->insertText("  ");
			textEditorModel->moveCursor(+2, +0);
		} else if (c == 13) {
			textEditorModel->enter();
		} else if (c == 8) {
			textEditorModel->backspaceChar();
		}
		return 0;
	}
	case WM_KEYDOWN:
		if (wParam == VK_DELETE) {
			textEditorModel->deleteChar();
			return 0;
		} else if (wParam == VK_UP) {
			textEditorModel->moveCursor(+0, -1);
			return 0;
		} else if (wParam == VK_DOWN) {
			textEditorModel->moveCursor(+0, +1);
			return 0;
		} else if (wParam == VK_LEFT) {
			textEditorModel->moveCursor(-1, +0);
			return 0;
		} else if (wParam == VK_RIGHT) {
			textEditorModel->moveCursor(+1, +0);
			return 0;
		} else if (wParam == VK_HOME) {
			textEditorModel->moveCursor(-textEditorModel->getCursorColumn(), +0);
			return 0;
		} else if (wParam == VK_END) {
			textEditorModel->moveCursor(textEditorModel->getLine(textEditorModel->getCursorLine()).length() -
					                    textEditorModel->getCursorColumn(), +0);
			return 0;
		} else if (wParam == VK_F1) {
			loadShader();
			return 0;
		} else if (wParam == VK_F2) {
			saveShader();
			return 0;
		} else if (wParam == VK_F5) {
			compileShader();
			return 0;
		} else if (wParam == VK_F4) {
			showEditor = !showEditor;
			return 0;
		}
		break;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

static char* szAppName = "GLSL Live Editor";

HWND makeWindow() {
	WNDCLASS wndclass;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = wndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = GetModuleHandle(NULL);
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass)) {
		return 0;
	}

	HWND hwnd = CreateWindow(szAppName,
		TEXT("GLSL Live Editor"),
		WS_POPUP | WS_VISIBLE | WS_MAXIMIZE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		GetModuleHandle(NULL),
		NULL);
	
	SetFocus(hwnd);

	return hwnd;
}

extern "C"
void WinMainCRTStartup()
{
	//hDC = GetDC(CreateWindow("edit", 0, WS_POPUP | WS_VISIBLE | WS_MAXIMIZE, 0, 0, 0, 0, 0, 0, 0, 0));
	HWND hwnd = makeWindow();
	hDC = GetDC(hwnd);
	ShowCursor(false);
	// init OpenGL
	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
	wglMakeCurrent(hDC, wglCreateContext(hDC));
	initGl();
	initStartingCode();
	initShaders();
	net_radio_init(hwnd);
	net_radio_openUrl("http://www.subflow.net:8000/listen.pls");
	bool net_radio_buffering = true;
	RECT rect;
	GetWindowRect(WindowFromDC(hDC), &rect);
	reshape(rect.right - rect.left, rect.bottom - rect.top);
	// main loop
	MSG msg;
	do {
		if (net_radio_buffering) {
			net_radio_buffering = !net_radio_updateBuffering();
		}

		while (PeekMessage(&msg, hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		render();
		SwapBuffers(hDC);
	} while (!GetAsyncKeyState(VK_ESCAPE));
	net_radio_free();
	finalShaders();
	finalGl();
	ExitProcess(0);
}
