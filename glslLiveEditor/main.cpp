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
#include "Camera.h"

// Minimal TechnoStream
// http://www.subflow.net:8000/listen.pls

#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "GLU32.lib")

static HWND hwnd = NULL;
static HDC hDC = NULL;

static TextEditorModel* textEditorModel;
static TextEditorUI* textEditorUI;
static TextEditor* textEditor;

static Camera* camera = NULL;

static bool cameraMovingForward = false;
static bool cameraMovingBack = false;
static bool cameraMovingLeft = false;
static bool cameraMovingRight = false;
static bool cameraMovingUp = false;
static bool cameraMovingDown = false;

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
	glDisable(GL_DEPTH_TEST);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	buildFont(hDC);
	textEditorModel = new DefaultTextEditorModel();
	textEditorUI = new OpenGLTextEditorUI();
	textEditor = new TextEditor(textEditorModel, textEditorUI);
}

void initStartingCode() {
	textEditorModel->insertText("#ifdef GL_ES\n");
	textEditorModel->insertText("precision highp float;\n");
	textEditorModel->insertText("#endif\n");
	textEditorModel->insertText("\n");
	textEditorModel->insertText("varying vec3 camV, camEP;\n");
	textEditorModel->insertText("uniform vec3 unResolution;\n");
	textEditorModel->insertText("uniform vec3 cameraForward;\n");
	textEditorModel->insertText("uniform vec3 cameraUp;\n");
	textEditorModel->insertText("uniform vec3 cameraPos;\n");
	textEditorModel->insertText("uniform float time;\n");
	textEditorModel->insertText("uniform sampler2D tex0;\n");
	textEditorModel->insertText("uniform sampler2D tex1;\n");
	textEditorModel->insertText("uniform sampler2D fft;\n");
	textEditorModel->insertText("uniform vec4 unPar;\n");
	textEditorModel->insertText("uniform vec4 unPos;\n");
	textEditorModel->insertText("uniform vec3 unBeatBassFFT;\n");
	textEditorModel->insertText("\n");
	textEditorModel->insertText("float fov = 75.0;\n");
	textEditorModel->insertText("float screenDist = 1.0 / tan(fov * 0.5);\n");
	textEditorModel->insertText("\n");
	textEditorModel->insertText("void getRay(out vec3 ro, out vec3 rd) {\n");
	textEditorModel->insertText("  vec2 uv = gl_FragCoord.xy / unResolution.xy * 2.0 - 1.0;\n");
	textEditorModel->insertText("  uv.x *= unResolution.x / unResolution.y;\n");
	textEditorModel->insertText("  vec3 r = cross(cameraForward, cameraUp);\n");
	textEditorModel->insertText("  ro = cameraPos;\n");
	textEditorModel->insertText("  rd = normalize(cameraForward * -screenDist + uv.x * r + uv.y * cameraUp);\n");
	textEditorModel->insertText("}\n");
	textEditorModel->insertText("\n");
	textEditorModel->insertText("void main() {\n");
	textEditorModel->insertText("  vec3 ro, rd;\n");
	textEditorModel->insertText("  getRay(ro, rd);\n");
	textEditorModel->insertText("  vec3 col = vec3(0.0);\n");
	textEditorModel->insertText("  \n");
	textEditorModel->insertText("  gl_FragColor = vec4(col, 1.0);\n");
	textEditorModel->insertText("}\n");
	textEditorModel->moveCursor(+2 - textEditorModel->getCursorColumn(), +32 - textEditorModel->getCursorLine());
}

PFNGLCREATESHADERPROC glCreateShader = NULL;
PFNGLCREATEPROGRAMPROC glCreateProgram = NULL;
PFNGLSHADERSOURCEPROC glShaderSource = NULL;
PFNGLCOMPILESHADERPROC glCompileShader = NULL;
PFNGLATTACHSHADERPROC glAttachShader = NULL;
PFNGLLINKPROGRAMPROC glLinkProgram = NULL;
PFNGLUSEPROGRAMPROC glUseProgram = NULL;
PFNGLUNIFORM1FPROC glUniform1f = NULL;
PFNGLUNIFORM3FPROC glUniform3f = NULL;
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
	glUniform3f = (PFNGLUNIFORM3FPROC)wglGetProcAddress("glUniform3f");
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

void updateCamera() {
	if (cameraMovingForward) {
		camera->moveForward(10.0);
	}
	if (cameraMovingLeft) {
		camera->moveLeft(10.0);
	}
	if (cameraMovingRight) {
		camera->moveRight(10.0);
	}
	if (cameraMovingBack) {
		camera->moveBack(10.0);
	}
	if (cameraMovingUp) {
		camera->moveUp(10.0);
	}
	if (cameraMovingDown) {
		camera->moveDown(10.0);
	}
	if (GetActiveWindow() != NULL) {
		static int oldMouseX=-1, oldMouseY=-1;
		int mouseX, mouseY;
		RECT rect;
		GetWindowRect(hwnd, &rect);
		int centreX = (rect.left + rect.right) >> 1;
		int centreY = (rect.top + rect.bottom) >> 1;
		POINT p;
		GetCursorPos(&p);
		mouseX = p.x - rect.left;
		mouseY = p.y - rect.top;
		if (mouseX != centreX || mouseY != centreY) {
			if (oldMouseX != -1) {
				int moveX = mouseX - oldMouseX;
				int moveY = mouseY - oldMouseY;
				camera->turnRight(moveX * 0.1);
				camera->turnDown(moveY * 0.1);
				SetCursorPos(centreX, centreY);
				mouseX = centreX - rect.left;
				mouseY = centreY - rect.top;
			}
		}
		oldMouseX = mouseX;
		oldMouseY = mouseY;
	}
}

static float screenWidth, screenHeight;

void setGLSLUniforms() {
	int time = glGetUniformLocation(shaderProgram, "time");
	int unResolution = glGetUniformLocation(shaderProgram, "unResolution");
	int cameraForward = glGetUniformLocation(shaderProgram, "cameraForward");
	int cameraUp = glGetUniformLocation(shaderProgram, "cameraUp");
	int cameraPos = glGetUniformLocation(shaderProgram, "cameraPos");
	glUniform1f(time, GetTickCount() / 1000.0f);
	RECT rect;
	GetWindowRect(hwnd, &rect);
	screenWidth = (float)(rect.right - rect.left);
	screenHeight = (float)(rect.bottom - rect.top);
	glUniform3f(unResolution, screenWidth, screenHeight, 0.0f);
	glUniform3f(cameraForward, (float)camera->getForward().getX(), (float)camera->getForward().getY(), (float)camera->getForward().getZ());
	glUniform3f(cameraUp, (float)camera->getUp().getX(), (float)camera->getUp().getY(), (float)camera->getUp().getZ());
	glUniform3f(cameraPos, (float)camera->getPosition().getX(), (float)camera->getPosition().getY(), (float)camera->getPosition().getZ());
}

void render() {
	if (shaderProgram != -1) {
		glUseProgram(shaderProgram);
		setGLSLUniforms();
		glColor3f(1.0f,0.0f,0.0f);
		glLoadIdentity();
		updateCamera();
		glScalef(screenWidth / screenHeight, 1.0f, 1.0f);
		glRects(-1,-1,1,1);
		glFlush();
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
		glTranslatef(-250.0f,120.0f,-200.0f);
		glPushMatrix();
		glColor3f(1.0f, 1.0f, 1.0f);
		glScalef(7.0f, 7.0f, 7.0f);
		glPrint("| F1: Load | F2: Save | F4: Show/Hide Code | F5: Compile | Esc: Exit | A/S/D/W/Mouse: Move |");
		glPopMatrix();
		glTranslatef(0.0f, -15.0f, 0.0f);
		glScalef(10.0f, 10.0f, 10.0f);
		textEditor->repaint();
		glFlush();
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

/*
// vertex shader
const GLchar *vsh="\
void main()\
{\
    gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\
}";*/

// vertex shader
const GLchar* vsh="\
varying vec3 camV,camEP;\
void main(){\
  gl_Position=gl_Vertex;\
  camV = vec3( gl_ModelViewMatrix*gl_Vertex);\
  camEP= vec3( gl_ModelViewMatrix*vec4(0,0,-1,1) );\
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
			textEditorUI->flashRed();
			int tmp;
			char buf[256];
			glGetShaderInfoLog(vertexShader, 255, &tmp, buf);
			buf[tmp] = 0;
			textEditorUI->setErrorText(buf);
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
		textEditorUI->flashRed();
		int tmp;
		char buf[256];
		glGetShaderInfoLog(fragmentShader, 255, &tmp, buf);
		buf[tmp] = 0;
		textEditorUI->setErrorText(buf);
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
		textEditorUI->flashRed();
		int tmp;
		char buf[256];
		glGetProgramInfoLog(shaderProgram, 255, &tmp, buf);
		buf[tmp] = 0;
		textEditorUI->setErrorText(buf);
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
	textEditorUI->setErrorText("");
	//vertexShader = glCreateShaderObject
}

PIXELFORMATDESCRIPTOR pfd={0, 0, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // wrong way ;)

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_CHAR: {
		if (showEditor) {
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
		}
		return 0;
	}
	case WM_KEYDOWN:
		if (showEditor) {
			if (wParam == VK_SHIFT) {
				textEditorModel->shiftDown();
				return 0;
			} else if (wParam == VK_DELETE) {
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
				showEditor = false;
				return 0;
			}
		} else {
			if (wParam == VK_F4) {
				showEditor = true;
				return 0;
			} else if (wParam == 'W') {
				cameraMovingForward = true;
				return 0;
			} else if (wParam == 'A') {
				cameraMovingLeft = true;
				return 0;
			} else if (wParam == 'D') {
				cameraMovingRight = true;
				return 0;
			} else if (wParam == 'S') {
				cameraMovingBack = true;
				return 0;
			} else if (wParam == VK_SHIFT) {
				cameraMovingUp = true;
				return 0;
			} else if (wParam == VK_CONTROL) {
				cameraMovingDown = true;
				return 0;
			}
		}
		break;
	case WM_KEYUP:
		if (showEditor) {
			if (wParam == VK_SHIFT) {
				textEditorModel->shiftUp();
				return 0;
			}
		} else {
			if (wParam == 'W') {
				cameraMovingForward = false;
				return 0;
			} else if (wParam == 'A') {
				cameraMovingLeft = false;
				return 0;
			} else if (wParam == 'D') {
				cameraMovingRight = false;
				return 0;
			} else if (wParam == 'S') {
				cameraMovingBack = false;
				return 0;
			} else if (wParam == VK_SHIFT) {
				cameraMovingUp = false;
				return 0;
			} else if (wParam == VK_CONTROL) {
				cameraMovingDown = false;
				return 0;
			}
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
	hwnd = makeWindow();
	hDC = GetDC(hwnd);
	ShowCursor(false);
	// init OpenGL
	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
	wglMakeCurrent(hDC, wglCreateContext(hDC));
	camera = new Camera();
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

		Sleep(10);
	} while (!GetAsyncKeyState(VK_ESCAPE));
	net_radio_free();
	finalShaders();
	finalGl();
	delete camera;
	ExitProcess(0);
}
