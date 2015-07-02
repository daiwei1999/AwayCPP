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
#include "PlaneGeometry.h"
#include "ATFTexture.h"
#include "TextureMaterial.h"
#include "OpenGLES2Context.h"
#include "ParticleAnimator.h"
#include "ParticleAnimationSet.h"
#include "ParticleAccelerationNode.h"
#include "ParticleRotateToPositionNode.h"
#include "ParticleVelocityNode.h"
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
away::DirectionalLight* light;
away::StaticLightPicker* lightPicker;

float lastPanAngle, lastTiltAngle;
int lastMouseX, lastMouseY;
bool move = false;

void initEngine()
{
	context = new away::OpenGLES2Context();
	view = new away::View3D(nullptr, nullptr, nullptr);
	view->setContext(context);
	view->setWidth(SCREEN_WIDTH);
	view->setHeight(SCREEN_HEIGHT);
	scene = view->getScene();
	cameraController = new away::HoverController(view->getCamera(), 45, 20, 1500, 0, 90);
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

void initScene()
{
	away::ByteArray* textureBytes = new away::ByteArray();
	readFileToByteArray("floor_diffuse.atf", textureBytes);
	away::Mesh* ground = new away::Mesh(new away::PlaneGeometry(2000, 2000, 40, 40), new away::TextureMaterial(new away::ATFTexture(textureBytes)));
	scene->addChild(ground);
}

void initParticle()
{
	away::PlaneGeometry* plane = new away::PlaneGeometry(50, 0, 1, 5, true, false, away::PlaneGeometryMode::BOTTOM_MIDDLE);
	std::vector<away::Geometry*> geometrySet;
	for (int i = 0; i < 1000; i++)
		geometrySet.push_back(plane);

	float gravity = 20;
	away::ParticleAnimationSet* animationSet = new away::ParticleAnimationSet(true, true, false, true);
	animationSet->m_initParticleFunc = [gravity](away::ParticleProperties& prop)
	{
		float velocity = away::MathUtils::random(100.f, 200.f);
		float elevation = away::MathUtils::random(0.f, away::MathConsts::PI);
		float azimuth = away::MathUtils::random(0.f, away::MathConsts::TWO_PI);
		float vx = velocity * std::cos(elevation);
		float vy = velocity * std::sin(elevation);
		prop.m_startTime = away::MathUtils::random(0.f, 10.f);
		prop.m_duration = 2 * vy / gravity;
		prop.m_lagTime = away::MathUtils::random(1.f, 2.f);
		prop[away::ParticleVelocityNode::VELOCITY_VECTOR3D].setTo(vx * std::cos(azimuth), vy, vx * std::sin(azimuth));
	};
	animationSet->addAnimation(new away::ParticleRotateToPositionNode(away::ParticlePropertiesMode::GLOBAL, &away::Vector3D::ZERO));
	animationSet->addAnimation(new away::ParticleVelocityNode(away::ParticlePropertiesMode::LOCAL_STATIC));
	animationSet->addAnimation(new away::ParticleAccelerationNode(away::ParticlePropertiesMode::GLOBAL, new away::Vector3D(0, -gravity, 0)));

	away::ByteArray* textureBytes = new away::ByteArray();
	readFileToByteArray("light.atf", textureBytes);
	away::TextureMaterial* material = new away::TextureMaterial(new away::ATFTexture(textureBytes));
	material->setBothSides(true);
	material->setBlendMode(away::BlendMode::ADD);

	away::Mesh* particleMesh = new away::Mesh(away::ParticleGeometryHelper::generateGeometry(geometrySet), material);
	particleMesh->setAnimator(new away::ParticleAnimator(animationSet));
	static_cast<away::ParticleAnimator*>(particleMesh->getAnimator())->start();

	away::AxisAlignedBoundingBox bounds;
	bounds.fromExtremes(-1500, 0, -1500, 1500, 1000, 1500);
	particleMesh->setBounds(&bounds);
	
	scene->addChild(particleMesh);
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
	gWindow = SDL_CreateWindow("ParticleFires", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

	away::AnimatorBase::update();
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
