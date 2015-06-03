#include <tchar.h>
#include "SDL.h"
#include "ByteArray.h"
#include "KLDParser.h"
#include "View3D.h"
#include "Scene3D.h"
#include "Camera3D.h"
#include "HoverController.h"
#include "ATFTexture.h"
#include "Mesh.h"
#include "Geometry.h"
#include "SphereGeometry.h"
#include "ColorMaterial.h"
#include "TextureMaterial.h"
#include "OpenGLES2Context.h"
#include "PointLight.h"
#include "StaticLightPicker.h"

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
away::Mesh* mesh;
away::Mesh* sphere;
away::HoverController* cameraController;
std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

float lastPanAngle, lastTiltAngle;
int lastMouseX, lastMouseY;
bool move = false;
away::PointLight* pointlLight;
away::StaticLightPicker* lightPicker;

void initEngine()
{
	context = new away::OpenGLES2Context();
	view = new away::View3D(nullptr, nullptr, nullptr);
	view->setContext(context);
	view->setWidth(SCREEN_WIDTH);
	view->setHeight(SCREEN_HEIGHT);
	scene = view->getScene();
	cameraController = new away::HoverController(view->getCamera(), 45, 10, 800);
}

void initLights()
{
	pointlLight = new away::PointLight();
	pointlLight->setColor(0xffddbb);
	pointlLight->setAmbient(1);

	sphere = new away::Mesh(new away::SphereGeometry(10), new away::ColorMaterial(0xffddbb));
	sphere->addChild(pointlLight);
	scene->addChild(sphere);

	away::LightVector lights;
	lights.push_back(pointlLight);
	lightPicker = new away::StaticLightPicker(lights);
}

void initScene()
{
	// load diffuse map
	away::ByteArray* diffuseBytes = new away::ByteArray();
	readFileToByteArray("head_diffuse.atf", diffuseBytes);
	away::ATFTexture* diffuseTexture = new away::ATFTexture(diffuseBytes);

	// load normal map
	away::ByteArray* normalBytes = new away::ByteArray();
	readFileToByteArray("head_normals.atf", normalBytes);
	away::ATFTexture* normalTexture = new away::ATFTexture(normalBytes);

	// load specular map
	away::ByteArray* specularBytes = new away::ByteArray();
	readFileToByteArray("head_specular.atf", specularBytes);
	away::ATFTexture* specularTexture = new away::ATFTexture(specularBytes);

	// load mesh file
	away::ByteArray kldBytes;
	readFileToByteArray("head.kld", &kldBytes);

	// parse mesh file
	away::TextureMaterial* material = new away::TextureMaterial(diffuseTexture);
	material->setNormalMap(normalTexture);
	material->setSpecularMap(specularTexture);
	material->setLightPicker(lightPicker);
	material->setGloss(10);
	material->setSpecular(0.8f);
	material->setAmbient(1);
	material->setAmbientColor(0x303040);
	
	mesh = new away::Mesh(new away::Geometry(), material);
	mesh->setY(-50);
	mesh->setRotationY(180);
	KLDParser parser(mesh);
	parser.parse(&kldBytes);
	scene->addChild(mesh);
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
	gWindow = SDL_CreateWindow("HeadTest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
	case SDLK_b:
		mesh->setShowBounds(!mesh->getShowBounds());
		break;
	case SDLK_LEFT:
		mesh->setX(mesh->getX() - 5);
		break;
	case SDLK_RIGHT:
		mesh->setX(mesh->getX() + 5);
		break;
	case SDLK_UP:
		mesh->setZ(mesh->getZ() + 5);
		break;
	case SDLK_DOWN:
		mesh->setZ(mesh->getZ() - 5);
		break;
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
	sphere->setX(500 * std::cos(angle));
	sphere->setZ(500 * std::sin(angle));
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
