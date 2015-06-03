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
#include "ByteArray.h"
#include "BitmapData.h"
#include "MapBinParser.h"
#include "View3D.h"
#include "Scene3D.h"
#include "Camera3D.h"
#include "OrthographicLens.h"
#include "ATFTexture.h"
#include "BitmapTexture.h"
#include "FirstPersonController.h"
#include "TerrainMesh.h"
#include "CubeGeometry.h"
#include "PlaneGeometry.h"
#include "TextureMaterial.h"
#include "OpenGLES2Context.h"
#include "TextureUtils.h"
#include "TextureProjector.h"
#include "ProjectiveTextureMethod.h"
#include "Segment.h"
#include "SegmentSet.h"
#include "WireframeTriangles.h"

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
	away::FirstPersonController* m_cameraController;
	float m_lastMouseX;
	float m_lastMouseY;
	float m_lastPanAngle;
	float m_lastTiltAngle;
	std::vector<TerrainMesh*> m_terrainMeshes;
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
	away::Camera3D* camera = engine->m_view->getCamera();
	camera->setY(1500);
	camera->getLens()->setFar(10000);
	engine->m_cameraController = new away::FirstPersonController(camera, 90, 0, -80, 80);
}

float getTerrainHeight(struct engine* engine, int col, int row, int numBlockPerRow, float x, float z)
{
	if (col == numBlockPerRow)
		col -= 1;

	if (row == numBlockPerRow)
		row -= 1;

	return engine->m_terrainMeshes[row * numBlockPerRow + col]->getTerrainHeight(x, z) + 1;
}

void initScene(struct engine* engine)
{
	// load map.bin
	away::ByteArray mapBytes;
	readFileToByteArray(engine, "map.bin", &mapBytes);

	// parse map.bin
	MapBinParser parser;
	parser.parse(&mapBytes);

	// load heightmap
	away::ByteArray heightBytes;
	readFileToByteArray(engine, "heightmap.dat", &heightBytes);

	// load blend texture
	away::ByteArray* blendBytes = new away::ByteArray();
	readFileToByteArray(engine, "blendmask.atf", blendBytes);
	away::ATFTexture* blendMap = new away::ATFTexture(blendBytes);

	// load surface textures
	std::vector<away::ATFTexture*> surfaceMaps;
	std::vector<SurfaceData>& surfaces = parser.getSurfaceData();
	for (SurfaceData& surface : surfaces)
	{
		away::ByteArray* surfaceBytes = new away::ByteArray();
		std::string diffuseMapPath = surface.m_diffuseMap + ".atf";
		readFileToByteArray(engine, diffuseMapPath.c_str(), surfaceBytes);
		surfaceMaps.push_back(new away::ATFTexture(surfaceBytes));
	}

	// load lightmap textures
	char name[16];
	std::vector<away::ATFTexture*> lightMaps;
	std::vector<LightMapData>& terrainLightMaps = parser.getTerrainLightMapData();
	for (unsigned int i = 0; i < terrainLightMaps.size(); i++)
	{
		snprintf(name, 16, "terrain%d.atf", i);
		away::ByteArray* lightMapBytes = new away::ByteArray();
		readFileToByteArray(engine, name, lightMapBytes);
		lightMaps.push_back(new away::ATFTexture(lightMapBytes));
	}

	// triangle indices
	unsigned short idx0, idx1;
	int sectorGrid = parser.getSectorGrid();
	int blockGrid = parser.getBlockGrid();
	float vertexSpace = parser.getVertexSpace();
	float sectorSize = sectorGrid * vertexSpace;
	int numVertexPerRow = blockGrid + 1;
	unsigned short* indices = new unsigned short[blockGrid * blockGrid * 6];
	for (int row = 0, i = 0; row < blockGrid; row++)
	{
		for (int col = 0; col < blockGrid; col++)
		{
			idx0 = numVertexPerRow * row + col;
			idx1 = idx0 + numVertexPerRow;
			indices[i++] = idx0;
			indices[i++] = idx1;
			indices[i++] = idx0 + 1;
			indices[i++] = idx0 + 1;
			indices[i++] = idx1;
			indices[i++] = idx1 + 1;
		}
	}

	// read walkgrid.dat and draw walkgrid to bitmap
	//ByteArray walkGridBytes;
	//readFileToByteArray(engine, "walkgrid.dat", &walkGridBytes);
	//int gridSize = parser.getWalkGridSize();
	//int bmdSize = TextureUtils::getBestPowerOf2(gridSize);
	//BitmapData* walkGridBmd = new BitmapData(bmdSize, bmdSize, false);
	//int margin = (bmdSize - gridSize) / 2;
	//for (int y = 0; y < gridSize; y++)
	//{
	//	for (int x = 0; x < gridSize; x++)
	//	{
	//		if (walkGridBytes.readUnsignedByte() & 0x80)
	//			walkGridBmd->setPixel(x + margin, y + margin, 0xFF0000);
	//		else
	//			walkGridBmd->setPixel(x + margin, y + margin, 0x00FF00);
	//	}
	//}

	//TextureProjector* projector = new TextureProjector(new BitmapTexture(walkGridBmd, false, false), false);
	//((OrthographicLens*)projector->getLens())->setProjectionHeight(sectorSize * bmdSize / gridSize);
	//method = new ProjectiveTextureMethod(projector, ProjectiveMode::MULTIPLY, false);

	// read walkmesh.dat and build WireframeTriangles
	away::ByteArray walkMeshBytes;
	readFileToByteArray(engine, "walkmesh.dat", &walkMeshBytes);

	away::Vector3D point;
	away::WireframeTriangles* walkableMesh = new away::WireframeTriangles(0x0000FF);
	unsigned int numMeshes = walkMeshBytes.readUnsignedInt();
	for (unsigned int i = 0; i < numMeshes; i++)
	{
		unsigned int numPoints = walkMeshBytes.readUnsignedInt() * 3;
		walkMeshBytes.setPosition(walkMeshBytes.getPosition() + 24);
		for (unsigned int j = 0; j < numPoints; j++)
		{
			point.m_x = walkMeshBytes.readFloat();
			point.m_y = walkMeshBytes.readFloat();
			point.m_z = walkMeshBytes.readFloat();
			walkableMesh->addPoint(point);
		}
	}
	engine->m_scene->addChild(walkableMesh);

	// build blocks
	int blockId = 0;
	int numBlockPerRow = sectorGrid / blockGrid;
	for (BlockData& blockData : parser.getBlockData())
	{
		// build terrain
		std::vector<away::ATFTexture*> layerMaps;
		std::vector<float> layerTilings, distMaskStrengths;
		for (int index : blockData.m_surfaceIndents)
		{
			layerMaps.push_back(surfaceMaps[index]);
			layerTilings.push_back(surfaces[index].m_textureTiling);
			distMaskStrengths.push_back(surfaces[index].m_distMaskStrength);
		}

		BeastData& beastData = blockData.m_terrainBeast;
		int frameIndex = beastData.m_frameIndex;

		TerrainMesh* terrainMesh = new TerrainMesh(blockGrid, vertexSpace);
		terrainMesh->buildGeometry(heightBytes, sectorGrid, parser.getMinHeight(), parser.getMaxHeight(), indices, blockId);
		terrainMesh->buildMaterial(blendMap, layerMaps, layerTilings, distMaskStrengths, lightMaps[frameIndex], beastData, terrainLightMaps[frameIndex], numBlockPerRow, blockId++);
		engine->m_scene->addChild(terrainMesh);
		engine->m_terrainMeshes.push_back(terrainMesh);
	}

	// build blockGrid
	away::SegmentSet* segmentSet = new away::SegmentSet();
	float x, z, blockSize = blockGrid * vertexSpace, halfSectorSize = sectorSize * 0.5f;
	for (int i = 1; i < numBlockPerRow; i++)
	{
		x = i * blockSize - halfSectorSize;
		for (int j = 0; j < sectorGrid; j++)
		{
			z = j * vertexSpace - halfSectorSize;
			away::Vector3D start(x, getTerrainHeight(engine, i, j / blockGrid, numBlockPerRow, x, z), z);
			z += vertexSpace;
			away::Vector3D end(x, getTerrainHeight(engine, i, (j + 1) / blockGrid, numBlockPerRow, x, z), z);
			segmentSet->addSegment(new away::Segment(start, end, 0xffffff, 0xffffff));
		}
	}
	for (int i = 1; i < numBlockPerRow; i++)
	{
		z = i * blockSize - halfSectorSize;
		for (int j = 0; j < sectorGrid; j++)
		{
			x = j * vertexSpace - halfSectorSize;
			away::Vector3D start(x, getTerrainHeight(engine, j / blockGrid, i, numBlockPerRow, x, z), z);
			x += vertexSpace;
			away::Vector3D end(x, getTerrainHeight(engine, (j + 1) / blockGrid, i, numBlockPerRow, x, z), z);
			segmentSet->addSegment(new away::Segment(start, end, 0xffffff, 0xffffff));
		}
	}
	engine->m_scene->addChild(segmentSet);
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
	initScene(engine);

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

/**
* Process the next input event.
*/
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	struct engine* engine = (struct engine*)app->userData;
	switch (AMotionEvent_getAction(event))
	{
	case AMOTION_EVENT_ACTION_DOWN:
		engine->m_lastMouseX = AMotionEvent_getX(event, 0);
		engine->m_lastMouseY = AMotionEvent_getY(event, 0);
		engine->m_lastPanAngle = engine->m_cameraController->getPanAngle();
		engine->m_lastTiltAngle = engine->m_cameraController->getTiltAngle();
		break;
	case AMOTION_EVENT_ACTION_MOVE:
		engine->m_cameraController->setPanAngle(0.3f * (AMotionEvent_getX(event, 0) - engine->m_lastMouseX) + engine->m_lastPanAngle);
		engine->m_cameraController->setTiltAngle(0.3f * (AMotionEvent_getY(event, 0) - engine->m_lastMouseY) + engine->m_lastTiltAngle);
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
