#include <tchar.h>
#include "SDL.h"
#include "ByteArray.h"
#include "View3D.h"
#include "Scene3D.h"
#include "Camera3D.h"
#include "HoverController.h"
#include "DirectionalLight.h"
#include "StaticLightPicker.h"
#include "Mesh.h"
#include "MathUtils.h"
#include "BitmapData.h"
#include "PlaneGeometry.h"
#include "ColorMaterial.h"
#include "OpenGLES2Context.h"
#include "ParticleAnimator.h"
#include "ParticleAnimationSet.h"
#include "ParticleBillboardNode.h"
#include "ParticleBezierCurveNode.h"
#include "ParticlePositionNode.h"
#include "ParticleGeometryHelper.h"
#include "ParticleGeometry.h"
#include "AxisAlignedBoundingBox.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

//The window we'll be rendering to
SDL_Window* gWindow;

//OpenGL context
SDL_GLContext gContext;

void readFileToByteArray(char* filename, away::ByteArray& bytes)
{
	std::ifstream vsIutput(filename, std::ios::in | std::ios::binary);
	if (vsIutput.is_open())
	{
		vsIutput.seekg(0, std::ios_base::end);
		bytes.setLength((unsigned int)vsIutput.tellg());
		vsIutput.seekg(0, std::ios_base::beg);
		vsIutput.read((char*)bytes.getByteData(), bytes.getLength());
		vsIutput.close();
	}
}

void readFileToBitmapData(char* filename, away::BitmapData& bmd)
{
	away::ByteArray bmpBytes;
	readFileToByteArray(filename, bmpBytes);
	bmpBytes.setPosition(54);

	unsigned char r, g, b;
	int width = bmd.getWidth();
	int height = bmd.getHeight();
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			b = bmpBytes.readUnsignedByte();
			g = bmpBytes.readUnsignedByte();
			r = bmpBytes.readUnsignedByte();
			bmd.setPixel(x, height - y - 1, (r << 16) | (g << 8) | b);
		}
	}
}

away::OpenGLES2Context* context;
away::View3D* view;
away::Scene3D* scene;
away::HoverController* cameraController;
away::DirectionalLight* light;
away::StaticLightPicker* lightPicker;
away::Mesh *redParticleMesh, *whiteParticleMesh;
away::ColorMaterial *redMaterial, *whiteMaterial;
std::vector<away::Vector3D> redPoints, whitePoints;
std::chrono::high_resolution_clock::time_point startTime = std::chrono::high_resolution_clock::now();

float lastPanAngle, lastTiltAngle;
int lastMouseX, lastMouseY;
bool move = false;

#define PARTICLE_SIZE 3

void initEngine()
{
	context = new away::OpenGLES2Context();
	view = new away::View3D(nullptr, nullptr, nullptr);
	view->setContext(context);
	view->setWidth(SCREEN_WIDTH);
	view->setHeight(SCREEN_HEIGHT);
	scene = view->getScene();
	cameraController = new away::HoverController(view->getCamera(), 225, 10, 1000);
}

void initLights()
{
	light = new away::DirectionalLight(-1, -1, 1);
	light->setAmbient(1);
	light->setAmbientColor(0x808080);
	light->setSpecular(0.3f);
	scene->addChild(light);

	away::LightVector lights = { light };
	lightPicker = new away::StaticLightPicker(lights);
}

void initParticle()
{
	// create red and white point vectors for the Adobe Flash Player image
	away::BitmapData bmd(128, 128, false);
	int width = bmd.getWidth();
	int height = bmd.getHeight();
	int halfWidth = width / 2;
	int halfHeight = height / 2;
	
	readFileToBitmapData("player.bmp", bmd);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			away::Vector3D point(PARTICLE_SIZE * (i - halfWidth - 100), PARTICLE_SIZE * (halfHeight - j));
			if (((bmd.getPixel(i, j) >> 8) & 0xff) <= 0xb0)
				redPoints.push_back(point);
			else
				whitePoints.push_back(point);
		}
	}

	// define where one logo stops and another starts
	int redSeparation = redPoints.size();
	int whiteSeparation = whitePoints.size();

	// create red and white point vectors for the Adobe AIR image
	readFileToBitmapData("air.bmp", bmd);
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			away::Vector3D point(PARTICLE_SIZE * (i - halfWidth + 100), PARTICLE_SIZE * (halfHeight - j));
			if (((bmd.getPixel(i, j) >> 8) & 0xff) <= 0xb0)
				redPoints.push_back(point);
			else
				whitePoints.push_back(point);
		}
	}

	int numRedPoints = redPoints.size();
	int numWhitePoints = whitePoints.size();

	away::PlaneGeometry* plane = new away::PlaneGeometry(PARTICLE_SIZE, PARTICLE_SIZE, 1, 1, false);
	std::vector<away::Geometry*> redGeometrySet, whiteGeometrySet;
	for (int i = 0; i < numRedPoints; i++)
		redGeometrySet.push_back(plane);

	for (int i = 0; i < numWhitePoints; i++)
		whiteGeometrySet.push_back(plane);

	// create read particle mesh
	away::ParticleAnimationSet* redAnimationSet = new away::ParticleAnimationSet(true);
	redAnimationSet->addAnimation(new away::ParticleBillboardNode());
	redAnimationSet->addAnimation(new away::ParticleBezierCurveNode(away::ParticlePropertiesMode::LOCAL_STATIC));
	redAnimationSet->addAnimation(new away::ParticlePositionNode(away::ParticlePropertiesMode::LOCAL_STATIC));
	redAnimationSet->m_initParticleFunc = [redSeparation](away::ParticleProperties& prop)
	{
		prop.m_startTime = 0;
		prop.m_duration = 1;
		
		if (prop.m_index < redSeparation)
			prop[away::ParticleBezierCurveNode::BEZIER_END_VECTOR3D].setTo(200 * PARTICLE_SIZE, 0, 0);
		else
			prop[away::ParticleBezierCurveNode::BEZIER_END_VECTOR3D].setTo(-200 * PARTICLE_SIZE, 0, 0);

		float r = 500;
		float degree1 = away::MathUtils::random(0.f, away::MathConsts::TWO_PI);
		float degree2 = away::MathUtils::random(0.f, away::MathConsts::TWO_PI);
		prop[away::ParticleBezierCurveNode::BEZIER_CONTROL_VECTOR3D].setTo(r * std::sin(degree1) * std::cos(degree2), r * std::cos(degree1) * std::cos(degree2), 2 * r * std::sin(degree2));
		prop[away::ParticlePositionNode::POSITION_VECTOR3D].copyFrom(redPoints[prop.m_index]);
	};

	away::ColorMaterial* redMaterial = new away::ColorMaterial(0xBE0E0E);
	redMaterial->setBothSides(true);
	redParticleMesh = new away::Mesh(away::ParticleGeometryHelper::generateGeometry(redGeometrySet), redMaterial);
	redParticleMesh->setAnimator(new away::ParticleAnimator(redAnimationSet));

	// create white particle mesh
	away::ParticleAnimationSet* whiteAnimationSet = new away::ParticleAnimationSet();
	whiteAnimationSet->addAnimation(new away::ParticleBillboardNode());
	whiteAnimationSet->addAnimation(new away::ParticleBezierCurveNode(away::ParticlePropertiesMode::LOCAL_STATIC));
	whiteAnimationSet->addAnimation(new away::ParticlePositionNode(away::ParticlePropertiesMode::LOCAL_STATIC));
	whiteAnimationSet->m_initParticleFunc = [whiteSeparation](away::ParticleProperties& prop)
	{
		prop.m_startTime = 0;
		prop.m_duration = 1;

		if (prop.m_index < whiteSeparation)
			prop[away::ParticleBezierCurveNode::BEZIER_END_VECTOR3D].setTo(200 * PARTICLE_SIZE, 0, 0);
		else
			prop[away::ParticleBezierCurveNode::BEZIER_END_VECTOR3D].setTo(-200 * PARTICLE_SIZE, 0, 0);

		float r = 500;
		float degree1 = away::MathUtils::random(0.f, away::MathConsts::TWO_PI);
		float degree2 = away::MathUtils::random(0.f, away::MathConsts::TWO_PI);
		prop[away::ParticleBezierCurveNode::BEZIER_CONTROL_VECTOR3D].setTo(r * std::sin(degree1) * std::cos(degree2), r * std::cos(degree1) * std::cos(degree2), r * std::sin(degree2));
		prop[away::ParticlePositionNode::POSITION_VECTOR3D].copyFrom(whitePoints[prop.m_index]);
	};

	away::ColorMaterial* whiteMaterial = new away::ColorMaterial(0xBEBEBE);
	whiteMaterial->setBothSides(true);
	whiteParticleMesh = new away::Mesh(away::ParticleGeometryHelper::generateGeometry(whiteGeometrySet), whiteMaterial);
	whiteParticleMesh->setAnimator(new away::ParticleAnimator(whiteAnimationSet));

	// set custom bounding box
	away::AxisAlignedBoundingBox bounds;
	bounds.fromExtremes(-1000, -1000, -1000, 1000, 1000, 1000);
	redParticleMesh->setBounds(&bounds);
	whiteParticleMesh->setBounds(&bounds);
	
	// add particle meshes to scene
	scene->addChild(redParticleMesh);
	scene->addChild(whiteParticleMesh);
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
	gWindow = SDL_CreateWindow("ParticleExplosions", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
	//initLights();
	initParticle();

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

	int time = 1000 * (std::sin(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime).count() * .0002f) + 1);
	static_cast<away::ParticleAnimator*>(redParticleMesh->getAnimator())->update(time);
	static_cast<away::ParticleAnimator*>(whiteParticleMesh->getAnimator())->update(time);
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
