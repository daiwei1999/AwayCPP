#include <tchar.h>
#include "SDL.h"
#include "ByteArray.h"
#include "View3D.h"
#include "Scene3D.h"
#include "Camera3D.h"
#include "OrthographicLens.h"
#include "FirstPersonController.h"
#include "ATFTexture.h"
#include "BitmapTexture.h"
#include "CompactSubGeometry.h"
#include "BoundingVolumeBase.h"
#include "TextureMaterial.h"
#include "TerrainDiffuseMethod.h"
#include "OpenGLES2Context.h"
#include "MapBinParser.h"
#include "TerrainMesh.h"
#include "TextureProjector.h"
#include "ProjectiveTextureMethod.h"
#include "TextureUtils.h"
#include "BitmapData.h"
#include "Segment.h"
#include "SegmentSet.h"
#include "WireframeTriangles.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 768;

//The window we'll be rendering to
SDL_Window* gWindow;

//OpenGL context
SDL_GLContext gContext;

void readFileToByteArray(const char* filename, away::ByteArray* bytes)
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
	else
		std::cout << filename << " not exist!" << std::endl;
}

away::OpenGLES2Context* context;
away::View3D* view;
away::Scene3D* scene;
away::Camera3D* camera;
away::FirstPersonController* cameraController;
std::vector<TerrainMesh*> terrainMeshes;
away::SegmentSet* segmentSet;
away::ProjectiveTextureMethod* method;
away::WireframeTriangles* walkableMesh;

float lastPanAngle, lastTiltAngle;
float drag = 0.5f;
float walkSpeed = 0;
float strafeSpeed = 0;
float walkAcceleration = 0;
float strafeAcceleration = 0;
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
	camera = view->getCamera();
	camera->setY(1500);
	camera->getLens()->setFar(10000);
	cameraController = new away::FirstPersonController(camera, 90, 0, -80, 80);
}

float getTerrainHeight(int col, int row, int numBlockPerRow, float x, float z)
{
	if (col == numBlockPerRow)
		col -= 1;

	if (row == numBlockPerRow)
		row -= 1;

	return terrainMeshes[row * numBlockPerRow + col]->getTerrainHeight(x, z) + 1;
}

void initScene()
{
	// load map.bin
	away::ByteArray mapBytes;
	readFileToByteArray("map.bin", &mapBytes);

	// parse map.bin
	MapBinParser parser;
	parser.parse(&mapBytes);

	// load heightmap
	away::ByteArray heightBytes;
	readFileToByteArray("heightmap.dat", &heightBytes);

	// load blend texture
	away::ByteArray* blendBytes = new away::ByteArray();
	readFileToByteArray("blendmask.atf", blendBytes);
	away::ATFTexture* blendMap = new away::ATFTexture(blendBytes);

	// load surface textures
	std::vector<away::ATFTexture*> surfaceMaps;
	std::vector<SurfaceData>& surfaces = parser.getSurfaceData();
	for (SurfaceData& surface : surfaces)
	{
		away::ByteArray* surfaceBytes = new away::ByteArray();
		std::string diffuseMapPath = surface.m_diffuseMap + ".atf";
		readFileToByteArray(diffuseMapPath.c_str(), surfaceBytes);
		surfaceMaps.push_back(new away::ATFTexture(surfaceBytes));
	}

	// load lightmap textures
	std::vector<away::ATFTexture*> lightMaps;
	std::vector<LightMapData>& terrainLightMaps = parser.getTerrainLightMapData();
	for (unsigned int i = 0; i < terrainLightMaps.size(); i++)
	{
		char name[16];
		sprintf_s(name, 16, "terrain%d.atf", i);
		away::ByteArray* lightMapBytes = new away::ByteArray();
		readFileToByteArray(name, lightMapBytes);
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
	away::ByteArray walkGridBytes;
	readFileToByteArray("walkgrid.dat", &walkGridBytes);
	int gridSize = parser.getWalkGridSize();
	int bmdSize = away::TextureUtils::getBestPowerOf2(gridSize);
	away::BitmapData* walkGridBmd = new away::BitmapData(bmdSize, bmdSize, false);
	int margin = (bmdSize - gridSize) / 2;
	for (int y = 0; y < gridSize; y++)
	{
		for (int x = 0; x < gridSize; x++)
		{
			if (walkGridBytes.readUnsignedByte() & 0x80)
				walkGridBmd->setPixel(x + margin, y + margin, 0xFF0000);
			else
				walkGridBmd->setPixel(x + margin, y + margin, 0x00FF00);
		}
	}

	away::TextureProjector* projector = new away::TextureProjector(new away::BitmapTexture(walkGridBmd, false, false), false);
	static_cast<away::OrthographicLens*>(projector->getLens())->setProjectionHeight(sectorSize * bmdSize / gridSize);
	method = new away::ProjectiveTextureMethod(projector, away::ProjectiveMode::MULTIPLY, false);

	// read walkmesh.dat and build WireframeTriangles
	away::ByteArray walkMeshBytes;
	readFileToByteArray("walkmesh.dat", &walkMeshBytes);

	away::Vector3D point;
	walkableMesh = new away::WireframeTriangles(0x0000FF);
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
		scene->addChild(terrainMesh);
		terrainMeshes.push_back(terrainMesh);
	}

	// build blockGrid
	segmentSet = new away::SegmentSet();
	float x, z, blockSize = blockGrid * vertexSpace, halfSectorSize = sectorSize * 0.5f;
	for (int i = 1; i < numBlockPerRow; i++)
	{
		x = i * blockSize - halfSectorSize;
		for (int j = 0; j < sectorGrid; j++)
		{
			z = j * vertexSpace - halfSectorSize;
			away::Vector3D start(x, getTerrainHeight(i, j / blockGrid, numBlockPerRow, x, z), z);
			z += vertexSpace;
			away::Vector3D end(x, getTerrainHeight(i, (j + 1) / blockGrid, numBlockPerRow, x, z), z);
			segmentSet->addSegment(new away::Segment(start, end, 0xffffff, 0xffffff));
		}
	}
	for (int i = 1; i < numBlockPerRow; i++)
	{
		z = i * blockSize - halfSectorSize;
		for (int j = 0; j < sectorGrid; j++)
		{
			x = j * vertexSpace - halfSectorSize;
			away::Vector3D start(x, getTerrainHeight(j / blockGrid, i, numBlockPerRow, x, z), z);
			x += vertexSpace;
			away::Vector3D end(x, getTerrainHeight((j + 1) / blockGrid, i, numBlockPerRow, x, z), z);
			segmentSet->addSegment(new away::Segment(start, end, 0xffffff, 0xffffff));
		}
	}
	scene->addChild(segmentSet);
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
	gWindow = SDL_CreateWindow("TerrainTest", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
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

void OnKeyDown(SDL_KeyboardEvent& event)
{
	switch (event.keysym.sym)
	{
	case SDLK_w:
	case SDLK_UP:
		walkAcceleration = 10;
		break;
	case SDLK_s:
	case SDLK_DOWN:
		walkAcceleration = -10;
		break;
	case SDLK_a:
	case SDLK_LEFT:
		strafeAcceleration = -10;
		break;
	case SDLK_d:
	case SDLK_RIGHT:
		strafeAcceleration = 10;
		break;
	case SDLK_EQUALS:
		context->setFillMode(away::FillMode::SOLID);
		break;
	case SDLK_MINUS:
		context->setFillMode(away::FillMode::WIREFRAME);
		break;
	case SDLK_b:
		if (segmentSet->getParent())
			scene->removeChild(segmentSet);
		else
			scene->addChild(segmentSet);
		break;
	case SDLK_l:
		for (auto terrainMesh : terrainMeshes)
		{
			away::TextureMaterial* material = static_cast<away::TextureMaterial*>(terrainMesh->getMaterial());
			away::TerrainDiffuseMethod* method = static_cast<away::TerrainDiffuseMethod*>(material->getDiffuseMethod());
			method->setUseLightMap(!method->getUseLightMap());
		}
		break;
	case SDLK_g:
		for (auto terrainMesh : terrainMeshes)
		{
			away::TextureMaterial* material = static_cast<away::TextureMaterial*>(terrainMesh->getMaterial());
			if (material->hasMethod(method))
				material->removeMethod(method);
			else
				material->addMethod(method);
		}
		break;
	case SDLK_m:
		if (walkableMesh->getParent())
			scene->removeChild(walkableMesh);
		else
			scene->addChild(walkableMesh);
		break;
	}
}

void onKeyUp(SDL_KeyboardEvent& event)
{
	switch (event.keysym.sym)
	{
	case SDLK_w:
	case SDLK_s:
	case SDLK_UP:
	case SDLK_DOWN:
		walkAcceleration = 0;
		break;
	case SDLK_a:
	case SDLK_d:
	case SDLK_LEFT:
	case SDLK_RIGHT:
		strafeAcceleration = 0;
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

	if (walkSpeed || walkAcceleration)
	{
		walkSpeed = (walkSpeed + walkAcceleration) * drag;
		if (std::abs(walkSpeed) < 0.01)
			walkSpeed = 0;
		cameraController->incrementWalk(walkSpeed);
	}

	if (strafeSpeed || strafeAcceleration)
	{
		strafeSpeed = (strafeSpeed + strafeAcceleration) * drag;
		if (std::abs(strafeSpeed) < 0.01)
			strafeSpeed = 0;
		cameraController->incrementStrafe(strafeSpeed);
	}

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
			else if (e.type == SDL_KEYDOWN)
			{
				OnKeyDown(e.key);
			}
			else if (e.type == SDL_KEYUP)
			{
				onKeyUp(e.key);
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
