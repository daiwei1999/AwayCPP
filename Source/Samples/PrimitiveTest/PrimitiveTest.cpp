#include <tchar.h>
#include "SDL.h"
#include "ByteArray.h"
#include "View3D.h"
#include "Scene3D.h"
#include "Camera3D.h"
#include "HoverController.h"
#include "DirectionalLight.h"
#include "StaticLightPicker.h"
#include "ATFTexture.h"
#include "Mesh.h"
#include "PlaneGeometry.h"
#include "CubeGeometry.h"
#include "TorusGeometry.h"
#include "SphereGeometry.h"
#include "CapsuleGeometry.h"
#include "CylinderGeometry.h"
#include "TextureMaterial.h"
#include "OpenGLES2Context.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

//The window we'll be rendering to
SDL_Window* gWindow;

//OpenGL context
SDL_GLContext gContext;

void readFileToByteArray(char* filename, away::ByteArray* bytes)
{
	std::ifstream vsIutput(filename, std::ios::in | std::ios::binary);
	if (vsIutput.is_open())
	{
		vsIutput.seekg(0, std::ios_base::end);
		bytes->setLength((unsigned int)vsIutput.tellg());
		vsIutput.seekg(0, std::ios_base::beg);
		vsIutput.read((char*)bytes->getByteData(), bytes->getLength());
		vsIutput.close();
	}
}

away::OpenGLES2Context* context;
away::View3D* view;
away::Scene3D* scene;
away::HoverController* cameraController;
away::DirectionalLight* light1;
away::DirectionalLight* light2;
away::StaticLightPicker* lightPicker;
away::Mesh* plane;
away::Mesh* cube;
away::Mesh* sphere;
away::Mesh* torus;

float lastPanAngle, lastTiltAngle;
int lastMouseX, lastMouseY;
bool move = false;
std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

void initEngine()
{
	context = new away::OpenGLES2Context();
	view = new away::View3D(nullptr, nullptr, nullptr);
	view->setContext(context);
	view->setWidth(SCREEN_WIDTH);
	view->setHeight(SCREEN_HEIGHT);
	scene = view->getScene();
	cameraController = new away::HoverController(view->getCamera(), 180, 20, 800);
}

void initLights()
{
	light1 = new away::DirectionalLight(0, -1, 0);
	light1->setAmbient(0.1f);
	light1->setDiffuse(0.7f);
	scene->addChild(light1);

	light2 = new away::DirectionalLight(0, -1, 0);
	light2->setColor(0x00FFFF);
	light2->setAmbient(0.1f);
	light2->setDiffuse(0.7f);
	scene->addChild(light2);

	away::LightVector lights = { light1, light2 };
	lightPicker = new away::StaticLightPicker(lights);
}

void initScene()
{
	// plane
	away::ByteArray* floorDiffuseBytes = new away::ByteArray();
	readFileToByteArray("floor_diffuse.atf", floorDiffuseBytes);
	away::ByteArray* floorSpecularBytes = new away::ByteArray();
	readFileToByteArray("floor_specular.atf", floorSpecularBytes);
	away::ByteArray* floorNormalBytes = new away::ByteArray();
	readFileToByteArray("floor_normal.atf", floorNormalBytes);

	away::TextureMaterial* planeMaterial = new away::TextureMaterial(new away::ATFTexture(floorDiffuseBytes));
	planeMaterial->setSpecularMap(new away::ATFTexture(floorSpecularBytes));
	planeMaterial->setNormalMap(new away::ATFTexture(floorNormalBytes));
	planeMaterial->setLightPicker(lightPicker);
	plane = new away::Mesh(new away::PlaneGeometry(1000, 1000, 2, 2), planeMaterial);
	plane->setY(-20);
	scene->addChild(plane);

	// cube
	away::ByteArray* trinketDiffuseBytes = new away::ByteArray();
	readFileToByteArray("trinket_diffuse.atf", trinketDiffuseBytes);
	away::ByteArray* trinketSpecularBytes = new away::ByteArray();
	readFileToByteArray("trinket_specular.atf", trinketSpecularBytes);
	away::ByteArray* trinketNormalBytes = new away::ByteArray();
	readFileToByteArray("trinket_normal.atf", trinketNormalBytes);

	away::TextureMaterial* cubeMaterial = new away::TextureMaterial(new away::ATFTexture(trinketDiffuseBytes));
	cubeMaterial->setSpecularMap(new away::ATFTexture(trinketSpecularBytes));
	cubeMaterial->setNormalMap(new away::ATFTexture(trinketNormalBytes));
	cubeMaterial->setLightPicker(lightPicker);
	cube = new away::Mesh(new away::CubeGeometry(200, 200, 200), cubeMaterial);
	cube->setX(300);
	cube->setY(160);
	cube->setZ(-250);
	scene->addChild(cube);

	// sphere
	away::ByteArray* ballDiffuseBytes = new away::ByteArray();
	readFileToByteArray("beachball_diffuse.atf", ballDiffuseBytes);
	away::ByteArray* ballSpecularBytes = new away::ByteArray();
	readFileToByteArray("beachball_specular.atf", ballSpecularBytes);

	away::TextureMaterial* sphereMaterial = new away::TextureMaterial(new away::ATFTexture(ballDiffuseBytes));
	sphereMaterial->setSpecularMap(new away::ATFTexture(ballSpecularBytes));
	sphereMaterial->setLightPicker(lightPicker);
	sphere = new away::Mesh(new away::SphereGeometry(150, 40, 20), sphereMaterial);
	sphere->setX(300);
	sphere->setY(160);
	sphere->setZ(300);
	scene->addChild(sphere);

	// torus
	away::ByteArray* weaveDiffuseBytes = new away::ByteArray();
	readFileToByteArray("weave_diffuse.atf", weaveDiffuseBytes);
	away::ByteArray* weaveNormalBytes = new away::ByteArray();
	readFileToByteArray("weave_normal.atf", weaveNormalBytes);

	away::ATFTexture* weaveDiffuseTexture = new away::ATFTexture(weaveDiffuseBytes);
	away::TextureMaterial* torusMaterial = new away::TextureMaterial(weaveDiffuseTexture);
	torusMaterial->setSpecularMap(weaveDiffuseTexture);
	torusMaterial->setNormalMap(new away::ATFTexture(weaveNormalBytes));
	torusMaterial->setLightPicker(lightPicker);
	torus = new away::Mesh(new away::TorusGeometry(150, 60, 40, 20), torusMaterial);
	torus->setX(-250);
	torus->setY(160);
	torus->setZ(-250);
	scene->addChild(torus);
}

bool init()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	// Create window
	gWindow = SDL_CreateWindow("PrimitiveTest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!gWindow)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Create context
	gContext = SDL_GL_CreateContext(gWindow);
	if (!gContext)
	{
		printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	// Use Vsync
	if (SDL_GL_SetSwapInterval(1) < 0)
	{
		printf("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	initEngine();
	initLights();
	initScene();

	return true;
}

void onMouseDown(SDL_MouseButtonEvent& event)
{
	SDL_GetMouseState(&lastMouseX, &lastMouseY);
	lastPanAngle = cameraController->getPanAngle();
	lastTiltAngle = cameraController->getTiltAngle();
	move = true;
}

void onMouseUp(SDL_MouseButtonEvent& event)
{
	move = false;
}

void onMouseWheel(SDL_MouseWheelEvent& event)
{
	cameraController->setDistance(cameraController->getDistance() + event.y * 5);
}

void OnKeyDown(SDL_KeyboardEvent& event)
{
	switch (event.keysym.sym)
	{
	case SDLK_EQUALS:
		context->setFillMode(away::FillMode::SOLID);
		break;
	case SDLK_MINUS:
		context->setFillMode(away::FillMode::WIREFRAME);
		break;
	}
}

void onWindowEvent(SDL_WindowEvent& event)
{
	if (event.event == SDL_WINDOWEVENT_RESIZED)
	{
		view->setWidth(event.data1);
		view->setHeight(event.data2);
	}
}

void render()
{
	if (move)
	{
		int x, y;
		SDL_GetMouseState(&x, &y);
		cameraController->setPanAngle(0.3f * (x - lastMouseX) + lastPanAngle);
		cameraController->setTiltAngle(0.3f * (y - lastMouseY) + lastTiltAngle);
	}

	float angle = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() * .001f;
	away::Vector3D direction(std::sin(angle) * 150000, 1000, std::cos(angle) * 150000);
	light1->setDirection(direction);
	context->clear(0, 0, 0);
	view->render();
}

void close()
{
	// Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = nullptr;

	// Quit SDL subsystems
	SDL_Quit();

	// Free memory
	delete context;
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (!init())
	{
		printf("Failed to initialize!\n");
		return 0;
	}

	bool quit = false;
	while (!quit)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				onMouseDown(e.button);
			}
			else if (e.type == SDL_MOUSEBUTTONUP)
			{
				onMouseUp(e.button);
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				onMouseWheel(e.wheel);
			}
			else if (e.type == SDL_KEYDOWN)
			{
				OnKeyDown(e.key);
			}
			else if (e.type == SDL_WINDOWEVENT)
			{
				onWindowEvent(e.window);
			}
		}

		// Render
		render();

		// Update screen
		SDL_GL_SwapWindow(gWindow);
	}

	// Free resources and close SDL
	close();

	return 0;
}
