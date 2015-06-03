#include <tchar.h>
#include "SDL.h"
#include "ByteArray.h"
#include "KLDParser.h"
#include "View3D.h"
#include "Scene3D.h"
#include "Camera3D.h"
#include "LensBase.h"
#include "HoverController.h"
#include "ATFTexture.h"
#include "Mesh.h"
#include "TextureMaterial.h"
#include "OpenGLES2Context.h"
#include "DirectionalLight.h"
#include "StaticLightPicker.h"
#include "SkeletonAnimator.h"
#include "JointTag.h"
#include "PlaneGeometry.h"
#include "ColorMaterial.h"
#include "HardShadowMapMethod.h"

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

away::OpenGLES2Context* context;
away::View3D* view;
away::Scene3D* scene;
away::HoverController* cameraController;
away::Mesh *character, *weapon, *ride;
float lastPanAngle, lastTiltAngle;
int lastMouseX, lastMouseY;
bool move = false;
away::DirectionalLight* directionalLight;
away::StaticLightPicker* lightPicker;

void initEngine()
{
	context = new away::OpenGLES2Context();
	view = new away::View3D(nullptr, nullptr, nullptr);
	view->setContext(context);
	view->setWidth(SCREEN_WIDTH);
	view->setHeight(SCREEN_HEIGHT);
	scene = view->getScene();
	away::Camera3D* camera = view->getCamera();
	camera->getLens()->setFar(1000);
	cameraController = new away::HoverController(camera, 180, 20, 400);
}

void initLights()
{
	directionalLight = new away::DirectionalLight(-1, -1, 1);
	directionalLight->setAmbient(1);
	directionalLight->setAmbientColor(0x808080);
	directionalLight->setSpecular(0.3f);
	scene->addChild(directionalLight);

	away::LightVector lights = { directionalLight };
	lightPicker = new away::StaticLightPicker(lights);
}

void initCharacter()
{
	// load texture
	away::ByteArray* textureBytes = new away::ByteArray();
	readFileToByteArray("mage_m0.atf", *textureBytes);
	away::ATFTexture* texture = new away::ATFTexture(textureBytes);

	// load mesh file
	away::ByteArray kldBytes;
	readFileToByteArray("mage_m0.kld", kldBytes);

	// parse mesh file
	character = new away::Mesh(new away::Geometry());
	character->setCastsShadows(true);
	KLDParser parser(character);
	parser.parse(&kldBytes);

	// build material
	away::TextureMaterial* material = new away::TextureMaterial(texture);
	material->setLightPicker(lightPicker);
	character->setMaterial(material);
	scene->addChild(character);
}

void initWeapon()
{
	// load texture
	away::ByteArray* textureBytes = new away::ByteArray();
	readFileToByteArray("m_weapon2.atf", *textureBytes);
	away::ATFTexture* texture = new away::ATFTexture(textureBytes);

	// load mesh file
	away::ByteArray kldBytes;
	readFileToByteArray("m_weapon2.kld", kldBytes);

	// parse mesh file
	weapon = new away::Mesh(new away::Geometry());
	weapon->setCastsShadows(true);
	KLDParser parser(weapon);
	parser.parse(&kldBytes);

	// build material
	away::TextureMaterial* material = new away::TextureMaterial(texture);
	material->setLightPicker(lightPicker);
	weapon->setMaterial(material);
}

void initRide()
{
	// load texture
	away::ByteArray* textureBytes = new away::ByteArray();
	readFileToByteArray("ride22.atf", *textureBytes);
	away::ATFTexture* texture = new away::ATFTexture(textureBytes);

	// load mesh file
	away::ByteArray kldBytes;
	readFileToByteArray("ride22.kld", kldBytes);

	// parse mesh file
	ride = new away::Mesh(new away::Geometry());
	ride->setCastsShadows(true);
	KLDParser parser(ride);
	parser.parse(&kldBytes);

	// build material
	away::TextureMaterial* material = new away::TextureMaterial(texture);
	material->setAlphaThreshold(0.5f);
	material->setLightPicker(lightPicker);
	ride->setMaterial(material);
}

void initGround()
{
	away::ColorMaterial* material = new away::ColorMaterial(0x888888);
	//material->setLightPicker(lightPicker);
	material->setShadowMethod(new away::HardShadowMapMethod(directionalLight));
	scene->addChild(new away::Mesh(new away::PlaneGeometry(1000, 1000), material));
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
	gWindow = SDL_CreateWindow("BasicTest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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
	initCharacter();
	initWeapon();
	initRide();
	initGround();

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

void playAnimation(const std::string& name)
{
	if (ride->getParent())
	{
		static_cast<away::SkeletonAnimator*>(ride->getAnimator())->play(name);
		static_cast<away::SkeletonAnimator*>(character->getAnimator())->play("ride" + name);
	}
	else
		static_cast<away::SkeletonAnimator*>(character->getAnimator())->play(name);
}

void OnKeyDown(SDL_KeyboardEvent& event)
{
	switch (event.keysym.sym)
	{
	case SDLK_LEFT:
		character->setX(character->getX() - 5);
		break;
	case SDLK_RIGHT:
		character->setX(character->getX() + 5);
		break;
	case SDLK_UP:
		character->setZ(character->getZ() + 5);
		break;
	case SDLK_DOWN:
		character->setZ(character->getZ() - 5);
		break;
	case SDLK_b:
		character->setShowBounds(!character->getShowBounds());
		break;
	case SDLK_1:
		playAnimation("attack1");
		break;
	case SDLK_2:
		playAnimation("attack2");
		break;
	case SDLK_3:
		playAnimation("attack3");
		break;
	case SDLK_4:
		playAnimation("attack4");
		break;
	case SDLK_5:
		playAnimation("attack5");
		break;
	case SDLK_6:
		playAnimation("attack6");
		break;
	case SDLK_7:
		playAnimation("attack7");
		break;
	case SDLK_8:
		playAnimation("attack8");
		break;
	case SDLK_9:
		playAnimation("attack9");
		break;
	case SDLK_0:
		playAnimation("attack10");
		break;
	case SDLK_c:
		playAnimation("collect");
		break;
	case SDLK_d:
		playAnimation("die");
		break;
	case SDLK_e:
		playAnimation("execute");
		break;
	case SDLK_h:
		playAnimation("hited");
		break;
	case SDLK_j:
		playAnimation("jump");
		break;
	case SDLK_k:
		playAnimation("kiss");
		break;
	case SDLK_r:
		playAnimation("run");
		break;
	case SDLK_s:
		playAnimation("stand");
		break;
	case SDLK_t:
		playAnimation("train");
		break;
	case SDLK_w:
		playAnimation("walk");
		break;
	case SDLK_m:
		if (ride->getParent())
		{
			scene->addChild(character);
			scene->removeChild(ride);
		}
		else
		{
			scene->addChild(ride);
			ride->getJointTagByName("ride")->addChild(character);
		}
		playAnimation("stand");
		break;
	case SDLK_SPACE:
		if (weapon->getParent())
			weapon->removeFromParent();
		else
			character->getJointTagByName("r_hand")->addChild(weapon);
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
