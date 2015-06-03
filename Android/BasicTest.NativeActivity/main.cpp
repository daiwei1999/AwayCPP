/*
* Copyright (C) 2010 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include <android/asset_manager.h>
#include "View3D.h"
#include "Scene3D.h"
#include "Camera3D.h"
#include "LensBase.h"
#include "HoverController.h"
#include "DirectionalLight.h"
#include "StaticLightPicker.h"
#include "Mesh.h"
#include "JointTag.h"
#include "PlaneGeometry.h"
#include "ColorMaterial.h"
#include "AxisAlignedBoundingBox.h"
#include "OpenGLES2Context.h"
#include "ByteArray.h"
#include "ATFTexture.h"
#include "KLDParser.h"
#include "SkeletonAnimator.h"
#include "TextureMaterial.h"
#include "HardShadowMapMethod.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

/**
* Our saved state data.
*/
struct saved_state {
	int32_t x;
	int32_t y;
};

/**
* Shared state for our app.
*/
struct engine {
	struct android_app* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	int animating;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	struct saved_state state;

	// additional data
	away::OpenGLES2Context* m_context;
	away::View3D* m_view;
	away::Scene3D* m_scene;
	away::HoverController* m_cameraController;
	away::StaticLightPicker* m_lightPicker;
	away::Mesh* m_character;
	away::Mesh* m_weapon;
	away::Mesh* m_ride;
	float m_lastMouseX;
	float m_lastMouseY;
	float m_lastPanAngle;
	float m_lastTiltAngle;
	float m_lastDistance;
};

void readFileToByteArray(struct engine* engine, const char* filename, away::ByteArray* bytes)
{
	AAsset* asset = AAssetManager_open(engine->app->activity->assetManager, filename, AASSET_MODE_UNKNOWN);
	if (asset)
	{
		off_t fileSize = AAsset_getLength(asset);
		bytes->setLength(fileSize);
		AAsset_read(asset, bytes->getByteData(), fileSize);
		AAsset_close(asset);
	}
}

void initEngine(struct engine* engine)
{
	engine->m_context = new away::OpenGLES2Context();
	engine->m_view = new away::View3D(nullptr, nullptr, nullptr);
	engine->m_view->setContext(engine->m_context);
	engine->m_view->setWidth(engine->width);
	engine->m_view->setHeight(engine->height);
	engine->m_scene = engine->m_view->getScene();
	engine->m_view->getCamera()->getLens()->setFar(1000);
	engine->m_cameraController = new away::HoverController(engine->m_view->getCamera(), 180, 20, 400, 10);
	engine->m_lastDistance = -1;
}

void initLights(struct engine* engine)
{
	away::DirectionalLight* directionalLight = new away::DirectionalLight(-1, -1, 1);
	directionalLight->setAmbient(1);
	directionalLight->setAmbientColor(0x808080);
	directionalLight->setSpecular(0.3f);
	engine->m_scene->addChild(directionalLight);

	away::LightVector lights = { directionalLight };
	engine->m_lightPicker = new away::StaticLightPicker(lights);
}

void initCharacter(struct engine* engine)
{
	// load texture
	away::ByteArray* textureBytes = new away::ByteArray();
	readFileToByteArray(engine, "mage_m0.atf", textureBytes);
	away::ATFTexture* texture = new away::ATFTexture(textureBytes);

	// load mesh file
	away::ByteArray kldBytes;
	readFileToByteArray(engine, "mage_m0.kld", &kldBytes);

	// parse mesh file
	engine->m_character = new away::Mesh(new away::Geometry());
	engine->m_character->setCastsShadows(true);
	KLDParser parser(engine->m_character);
	parser.parse(&kldBytes);

	// build material
	away::TextureMaterial* material = new away::TextureMaterial(texture);
	material->setLightPicker(engine->m_lightPicker);
	engine->m_character->setMaterial(material);
}

void initWeapon(struct engine* engine)
{
	// load texture
	away::ByteArray* textureBytes = new away::ByteArray();
	readFileToByteArray(engine, "m_weapon2.atf", textureBytes);
	away::ATFTexture* texture = new away::ATFTexture(textureBytes);

	// load mesh file
	away::ByteArray kldBytes;
	readFileToByteArray(engine, "m_weapon2.kld", &kldBytes);

	// parse mesh file
	engine->m_weapon = new away::Mesh(new away::Geometry());
	engine->m_weapon->setCastsShadows(true);
	KLDParser parser(engine->m_weapon);
	parser.parse(&kldBytes);

	// build material
	away::TextureMaterial* material = new away::TextureMaterial(texture);
	material->setLightPicker(engine->m_lightPicker);
	engine->m_weapon->setMaterial(material);
}

void initRide(struct engine* engine)
{
	// load texture
	away::ByteArray* textureBytes = new away::ByteArray();
	readFileToByteArray(engine, "ride22.atf", textureBytes);
	away::ATFTexture* texture = new away::ATFTexture(textureBytes);

	// load mesh file
	away::ByteArray kldBytes;
	readFileToByteArray(engine, "ride22.kld", &kldBytes);

	// parse mesh file
	engine->m_ride = new away::Mesh(new away::Geometry());
	engine->m_ride->setCastsShadows(true);
	KLDParser parser(engine->m_ride);
	parser.parse(&kldBytes);

	// build material
	away::TextureMaterial* material = new away::TextureMaterial(texture);
	material->setAlphaThreshold(0.5f);
	material->setLightPicker(engine->m_lightPicker);
	engine->m_ride->setMaterial(material);
}

void initGround(struct engine* engine)
{
	away::ColorMaterial* material = new away::ColorMaterial(0x888888);
	material->setShadowMethod(new away::HardShadowMapMethod(engine->m_lightPicker->getLights()[0]));
	engine->m_scene->addChild(new away::Mesh(new away::PlaneGeometry(1000, 1000), material));
}

void playAnimation(struct engine* engine, const std::string& name)
{
	if (engine->m_ride->getParent())
	{
		static_cast<away::SkeletonAnimator*>(engine->m_ride->getAnimator())->play(name);
		static_cast<away::SkeletonAnimator*>(engine->m_character->getAnimator())->play("ride" + name);
	}
	else
		static_cast<away::SkeletonAnimator*>(engine->m_character->getAnimator())->play(name);
}

/**
* Initialize an EGL context for the current display.
*/
static int engine_init_display(struct engine* engine) {
	// initialize OpenGL ES and EGL

	/*
	* Here specify the attributes of the desired configuration.
	* Below, we select an EGLConfig with at least 8 bits per color
	* component compatible with on-screen windows
	*/
	const EGLint attribs[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 16,
		EGL_NONE
	};
	EGLint w, h, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	* sample, we have a very simplified selection process, where we pick
	* the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	* As soon as we picked a EGLConfig, we can safely reconfigure the
	* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);

	const EGLint attribList[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
	context = eglCreateContext(display, config, EGL_NO_CONTEXT, attribList);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;

	initEngine(engine);
	initLights(engine);
	initCharacter(engine);
	initWeapon(engine);
	initRide(engine);
	initGround(engine);

	engine->m_scene->addChild(engine->m_ride);
	engine->m_ride->getJointTagByName("ride")->addChild(engine->m_character);
	engine->m_character->getJointTagByName("r_hand")->addChild(engine->m_weapon);
	playAnimation(engine, "stand");

	return 0;
}

/**
* Just the current frame in the display.
*/
static void engine_draw_frame(struct engine* engine) {
	if (engine->display == NULL) {
		// No display.
		return;
	}

	away::AnimatorBase::update();
	engine->m_context->clear(0, 0, 0);
	engine->m_view->render();

	eglSwapBuffers(engine->display, engine->surface);
}

/**
* Tear down the EGL context currently associated with the display.
*/
static void engine_term_display(struct engine* engine) {
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->animating = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

void recordCameraState(struct engine* engine, AInputEvent* event)
{
	engine->m_lastMouseX = AMotionEvent_getX(event, 0);
	engine->m_lastMouseY = AMotionEvent_getY(event, 0);
	engine->m_lastPanAngle = engine->m_cameraController->getPanAngle();
	engine->m_lastTiltAngle = engine->m_cameraController->getTiltAngle();
}

/**
* Process the next input event.
*/
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	struct engine* engine = (struct engine*)app->userData;
	switch (AMotionEvent_getAction(event))
	{
	case AMOTION_EVENT_ACTION_DOWN:
		recordCameraState(engine, event);
		break;
	case AMOTION_EVENT_ACTION_UP:
		engine->m_lastDistance = -1;
		break;
	case AMOTION_EVENT_ACTION_MOVE:
		if (AMotionEvent_getPointerCount(event) == 1)
		{
			if (engine->m_lastDistance > 0)
			{
				recordCameraState(engine, event);
				engine->m_lastDistance = -1;
			}
			else
			{
				engine->m_cameraController->setPanAngle(0.3f * (AMotionEvent_getX(event, 0) - engine->m_lastMouseX) + engine->m_lastPanAngle);
				engine->m_cameraController->setTiltAngle(0.3f * (AMotionEvent_getY(event, 0) - engine->m_lastMouseY) + engine->m_lastTiltAngle);
			}
		}
		else
		{
			float offsetX = AMotionEvent_getX(event, 0) - AMotionEvent_getX(event, 1);
			float offsetY = AMotionEvent_getY(event, 0) - AMotionEvent_getY(event, 1);
			float currentDistance = std::sqrt(offsetX * offsetX + offsetY * offsetY);
			if (engine->m_lastDistance > 0)
				engine->m_cameraController->setDistance(engine->m_cameraController->getDistance() - (currentDistance - engine->m_lastDistance) * .5f);
			engine->m_lastDistance = currentDistance;
		}
		break;
	}
	return 0;
}

/**
* Process the next main command.
*/
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		// The system has asked us to save our current state.  Do so.
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*)engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_INIT_WINDOW:
		// The window is being shown, get it ready.
		if (engine->app->window != NULL) {
			engine_init_display(engine);
			engine_draw_frame(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// The window is being hidden or closed, clean it up.
		engine_term_display(engine);
		break;
	case APP_CMD_GAINED_FOCUS:
		// When our app gains focus, we start monitoring the accelerometer.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
			// We'd like to get 60 events per second (in us).
			ASensorEventQueue_setEventRate(engine->sensorEventQueue,
				engine->accelerometerSensor, (1000L / 60) * 1000);
		}
		break;
	case APP_CMD_LOST_FOCUS:
		// When our app loses focus, we stop monitoring the accelerometer.
		// This is to avoid consuming battery while not being used.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
		}
		// Also stop animating.
		engine->animating = 0;
		engine_draw_frame(engine);
		break;
	}
}

/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(struct android_app* state) {
	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;

	// Prepare to monitor accelerometer
	engine.sensorManager = ASensorManager_getInstance();
	engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
		ASENSOR_TYPE_ACCELEROMETER);
	engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
		state->looper, LOOPER_ID_USER, NULL, NULL);

	if (state->savedState != NULL) {
		// We are starting with a previous saved state; restore from it.
		engine.state = *(struct saved_state*)state->savedState;
	}

	engine.animating = 1;

	// loop waiting for stuff to do.

	while (1) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
			(void**)&source)) >= 0) {

			// Process this event.
			if (source != NULL) {
				source->process(state, source);
			}

			// If a sensor has data, process it now.
			if (ident == LOOPER_ID_USER) {
				if (engine.accelerometerSensor != NULL) {
					ASensorEvent event;
					while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
						&event, 1) > 0) {
						LOGI("accelerometer: x=%f y=%f z=%f",
							event.acceleration.x, event.acceleration.y,
							event.acceleration.z);
					}
				}
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0) {
				engine_term_display(&engine);
				return;
			}
		}

		if (engine.animating) {
			// Drawing is throttled to the screen update rate, so there
			// is no need to do timing here.
			engine_draw_frame(&engine);
		}
	}
}
