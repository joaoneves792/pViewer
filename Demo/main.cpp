#include <iostream>
#include <sstream>
#include <cmath>

extern "C" {
#include <unistd.h>
#include <getopt.h>
}

#include "GL/glew.h"

#define GL_GLEXT_PROTOTYPES
#include "SDL.h"
#include "SDL_video.h"
#include "SDL_opengl.h"
#include "SDL_syswm.h"


#include "CGJengine.h"
#include "scene.h"
#include "actions.h"
#include "constants.h"
#include "pipeline.h"
#include "Browser.h"

#define CAPTION "CGJDemo"

int WinX = WIN_X;
int WinY = WIN_Y;
SDL_Window* WindowHandle = nullptr;
unsigned int FrameCount = 0;
int inVR = 0;
bool running = true;

/*
SDL_SysWMinfo info;
Display *sdl_display = NULL;
Window sdl_win = 0;
GLXContext sdl_gl_context = NULL;
*/

/////////////////////////////////////////////////////////////////////// CALLBACKS
void cleanup(){
    ResourceManager::getInstance()->destroyEverything();
    ResourceManager::deleteInstance();
}


void display(){
	++FrameCount;

	if(inVR){
		static VRCamera* cam = (VRCamera*)ResourceManager::getInstance()->getCamera(SPHERE_CAM);
		static ColorTextureFrameBuffer* leftFBO =
				(ColorTextureFrameBuffer*)ResourceManager::getInstance()->getFrameBuffer(LEFT_FBO_RENDER);
		static ColorTextureFrameBuffer* rightFBO =
				(ColorTextureFrameBuffer*)ResourceManager::getInstance()->getFrameBuffer(RIGHT_FBO_RENDER);

		cam->updatePose();
		cam->setCurrentEye(EYE_LEFT);
        executePipeline(leftFBO);
		cam->setCurrentEye(EYE_RIGHT);
		executePipeline(rightFBO);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		cam->submit(leftFBO, rightFBO);
        //executePipeline(nullptr);

	}else {
        executePipeline(nullptr);
    }
	checkOpenGLError("ERROR: Could not draw scene.");
	SDL_GL_SwapWindow(WindowHandle);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor( 0, 0, 0, 1 );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    glFlush();
    glFinish();
}

void update(int dt){
	static SceneGraph* scene = ResourceManager::getInstance()->getScene(SCENE);
	InputManager::update();
	scene->update(dt);
}

void timer(int value){
	std::ostringstream oss;
	oss << Browser::getInstance()->getCurrentName() << ": " << FrameCount << " FPS";
	std::string s = oss.str();
	SDL_SetWindowTitle(WindowHandle, s.c_str());
	FrameCount = 0;
}

void idle(){
    static long lastTime = InputManager::getTime();
    long currentTime = InputManager::getTime();
    int timeDelta = (int)(currentTime-lastTime);
    lastTime = currentTime;

    static int timer1 = 100;
    timer1 -= timeDelta;
    if(timer1 <= 0){
    	timer1 = 100;
    	timer(0);
    }

    update(timeDelta);
	display();
}

void resizeFBOs(int w, int h){
    ResourceManager::getInstance()->getCamera(SPHERE_CAM)->resize(w, h);
    ResourceManager::getInstance()->getFrameBuffer(SIDE_FBO1)->resize(w, h);
}
void reshape(int w, int h){
    if(w <= 0 || h <= 0)
		return;
	WinX = w;
    WinY = h;
    if(!inVR) {
        resizeFBOs(w, h);
	}
	glViewport(0, 0, w, h);
}


/////////////////////////////////////////////////////////////////////// INPUT
void keyboard(unsigned char key, int x, int y){
	InputManager::getInstance()->keyDown(key);
}

void keyboardUp(unsigned char key, int x, int y){
    InputManager::getInstance()->keyUp(key);
}

void specialKeyboard(int key, int x, int y){
    InputManager::getInstance()->specialKeyDown(key);
}

void specialKeyboardUp(int key, int x, int y){
    InputManager::getInstance()->specialKeyUp(key);
}

void mouse(int button, int state, int x, int y) {
    InputManager::getInstance()->mouseMovement(x, y);
    if(state == SDL_PRESSED) {
		InputManager::getInstance()->specialKeyDown(button);
    }else if(state == SDL_RELEASED){
		InputManager::getInstance()->specialKeyUp(button);
	}
}

/////////////////////////////////////////////////////////////////////// SETUP


void checkOpenGLInfo(){
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	std::cerr << "OpenGL Renderer: " << renderer << " (" << vendor << ")" << std::endl;
	std::cerr << "OpenGL version " << version << std::endl;
	std::cerr << "GLSL version " << glslVersion << std::endl;
    if(GLEW_EXT_texture_filter_anisotropic) {
		GLfloat largestAnisotropic;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largestAnisotropic);
		std::cerr << "Using anisotropic x" << largestAnisotropic << std::endl;
	}else{
		std::cerr << "Using linear filtering" << std::endl;
	}
}

void setupOpenGL(){
	checkOpenGLInfo();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	glDepthRange(0.0, 1.0);
	glClearDepth(1.0);

    /*Set transparency on diffuse render target*/
    glEnablei(GL_BLEND, 0);
    glBlendFunci(0, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
}

void setupGLEW() {
	glewExperimental = GL_TRUE;
	GLenum result = glewInit() ;
	if (result != GLEW_OK) {
		std::cerr << "ERROR glewInit: " << glewGetString(result) << std::endl;
		exit(EXIT_FAILURE);
	}
	//GLenum err_code = glGetError();
}

void setupSDL2(int argc, char** argv){
	SDL_GLContext context;

	if (SDL_Init (SDL_INIT_EVERYTHING) < 0) {
		std::cerr << "Unable to initialize SDL: " << SDL_GetError() << std::endl;
		exit(-1);
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	WindowHandle = SDL_CreateWindow(CAPTION, SDL_WINDOWPOS_CENTERED,
								 SDL_WINDOWPOS_CENTERED, WinX, WinY, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!WindowHandle){
		std::cerr << SDL_GetError() << std::endl;
		exit(-1);
	}

	context = SDL_GL_CreateContext(WindowHandle);
	if (!context){
		std::cerr << SDL_GetError() << std::endl;
		exit(-1);
	}

	//SDL_GL_SetSwapInterval(1); // VSync
}

void setupVR(){
	VRCamera* cam = (VRCamera*)ResourceManager::getInstance()->getCamera(SPHERE_CAM);

	ResourceManager::Factory::createColorTextureFrameBuffer(LEFT_FBO_RENDER,
			cam->getRecommendedWidth(), cam->getRecommendedHeight());

	ResourceManager::Factory::createColorTextureFrameBuffer(RIGHT_FBO_RENDER,
			cam->getRecommendedWidth(), cam->getRecommendedHeight());

	resizeFBOs(cam->getRecommendedWidth(), cam->getRecommendedHeight());
}

void init(int argc, char* argv[]){
	int c;
	int option_index = 0;
	std::string filename;
    struct option long_options[] = {
                    {"vr", 0, &inVR, 1},
                    {nullptr, 0, nullptr, 0}
            };

	while ((c = getopt_long(argc, argv, "f:", long_options, &option_index)) != -1)
		switch(c){
			case 0:
				break;
			case 'f':
				filename = std::string(optarg);
				std::cout << filename << std::endl;
				break;
			default:
				exit(-1);
		}
	if(inVR)
		std::cout << "VR Mode" << std::endl;

	if(filename.empty())
		exit(-1);

	setupSDL2(argc, argv);
	setupGLEW();
	setupOpenGL();

	InputManager::getInstance()->setActionInterval(10);

    setupScene();
	setupPipeline();
    setupActions();

	loadInput(filename);

	if(inVR){
		setupVR();
	}
	reshape(WinX, WinY);
}

void mainLoop(){
	while(running){
		SDL_Event evt;
		SDL_Keycode key;
		while (SDL_PollEvent(&evt)){
			switch(evt.type){
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					key = evt.key.keysym.sym;
                    if(key >= 'a' && key <= 'z' && evt.key.keysym.mod & KMOD_SHIFT)
	                   	key -= ('a'-'A');
					if(key < 256) {
						if (evt.type == SDL_KEYDOWN)
							keyboard((unsigned char) key, 0, 0);
						else
							keyboardUp((unsigned char) key, 0, 0);
					}else {
						if(evt.type == SDL_KEYDOWN)
							specialKeyboard((int) key, 0, 0);
						else
							specialKeyboardUp((int)key, 0, 0);
					}
					break;
				case SDL_MOUSEMOTION:
					mouse(0, 0, evt.motion.x, evt.motion.y);
					break;
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					mouse(evt.button.button, evt.button.state, evt.button.x, evt.button.y);
					break;
				case SDL_MOUSEWHEEL:
					mouse((evt.wheel.y > 0)?PVIEWER_MOUSE_WHEEL_UP:PVIEWER_MOUSE_WHEEL_DOWN, SDL_PRESSED, 0, 0);
					break;
				case SDL_WINDOWEVENT:
					switch (evt.window.event){
						case SDL_WINDOWEVENT_RESIZED:
						case SDL_WINDOWEVENT_SIZE_CHANGED:
						case SDL_WINDOWEVENT_MAXIMIZED:
						case SDL_WINDOWEVENT_RESTORED:
							reshape(evt.window.data1, evt.window.data2);
							break;
						case SDL_WINDOWEVENT_CLOSE:
							running = false;
							break;
						default:
							break;
					};
				default:
					break;
			};
		}
		idle();
	}
}


int main(int argc, char* argv[]){
	init(argc, argv);
	mainLoop();
	exit(EXIT_SUCCESS);
}

///////////////////////////////////////////////////////////////////////
