#pragma once

#include "Setup.h"

// STL including
#include <cstring>
#include <limits>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <unordered_map>
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <exception>
#include <chrono>
#include <functional>
#include <random>
#include <type_traits>

#define SAFE_DELETE(p) { if (p) { delete (p); (p) = nullptr; } }
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p); (p) = nullptr; } }

#define AWAY_NAMESPACE_BEGIN namespace away {
#define AWAY_NAMESPACE_END   }
#define USING_AWAY_NAMESPACE using namespace away;

#define DEFINE_CALLBACK_CLASS(classname, methodname) \
class _##methodname \
{ \
	classname* m_instance; \
public: \
	explicit _##methodname(classname* instance) : m_instance(instance) {} \
	friend bool operator==(const _##methodname& lhs, const _##methodname& rhs) { return lhs.m_instance == rhs.m_instance; } \
	void operator()(Event* event) const { m_instance->methodname(event); } \
};

#define CALLBACK_OBJECT(methodname) _##methodname(this)

AWAY_NAMESPACE_BEGIN

// Agal
class Shader;
class ShaderChunk;

// Animators
class AnimationRegisterCache;
class AnimationSubGeometry;
class ColorSegmentPoint;
class ParticleData;
class ParticleAnimationData;
class SkeletonJoint;
class Skeleton;
class JointPose;
class SkeletonPose;
class AnimationClipNodeBase;
class AnimationNodeBase;
class IAnimationNode;
class ParticleAccelerationNode;
class ParticleBezierCurveNode;
class ParticleBillboardNode;
class ParticleColorNode;
class ParticleDissolveNode;
class ParticleFollowNode;
class ParticleInitialColorNode;
class ParticleProperties;
class ParticleNodeBase;
class ParticleOrbitNode;
class ParticleOscillatorNode;
class ParticlePositionNode;
class ParticleRotateToHeadingNode;
class ParticleRotateToPositionNode;
class ParticleRotationalVelocityNode;
class ParticleScaleNode;
class ParticleSegmentedColorNode;
class ParticleSegmentedScaleNode;
class ParticleSpriteSheetNode;
class ParticleTimeNode;
class ParticleUVNode;
class ParticleVelocityNode;
class SkeletonClipNode;
class AnimationClipState;
class AnimationStateBase;
class IAnimationState;
class ParticleAccelerationState;
class ParticleBezierCurveState;
class ParticleBillboardState;
class ParticleColorState;
class ParticleDissolveState;
class ParticleFollowState;
class ParticleInitialColorState;
class ParticleOrbitState;
class ParticleOscillatorState;
class ParticlePositionState;
class ParticleRotateToHeadingState;
class ParticleRotateToPositionState;
class ParticleRotationalVelocityState;
class ParticleScaleState;
class ParticleSegmentedColorState;
class ParticleSegmentedScaleState;
class ParticleSpriteSheetState;
class ParticleStateBase;
class ParticleTimeState;
class ParticleUVState;
class ParticleVelocityState;
class SkeletonClipState;
class AnimationSetBase;
class AnimatorBase;
class IAnimationSet;
class IAnimator;
class ParticleAnimationSet;
class ParticleAnimator;
class SkeletonAnimationSet;
class SkeletonAnimator;

// Bounds
class AxisAlignedBoundingBox;
class BoundingSphere;
class BoundingVolumeBase;
class NullBounds;

// Cameras
class FreeMatrixLens;
class LensBase;
class OrthographicLens;
class PerspectiveLens;
class Camera3D;

// Containers
class ObjectContainer3D;
class Scene3D;
class View3D;

// Controllers
class ControllerBase;
class FirstPersonController;
class HoverController;
class LookAtController;

// Core
class CompactSubGeometry;
class EventDispatcher;
class Geometry;
class IMaterialOwner;
class IRenderable;
class ISubGeometry;
class Object3D;
class ParticleGeometry;
class SkinnedSubGeometry;
class SubGeometry;
class SubGeometryBase;
class SubMesh;
class RenderableListItem;
class RenderableListItemPool;
class EntityListItem;
class EntityListItemPool;
class AGALProgramCache;
class RTTBufferManager;
class CameraNode;
class DirectionalLightNode;
class EntityNode;
class LightNode;
class LightProbeNode;
class MeshNode;
class NodeBase;
class OctreeNode;
class Partition3D;
class PointLightNode;
class QuadtreeNode;
class RenderableNode;
class SkyBoxNode;
class PickingCollisionVO;
class BackgroundImageRenderer;
class DefaultRenderer;
class DepthRenderer;
class FilterRenderer;
class RendererBase;
class IEntitySorter;
class RenderableMergeSort;
class EntityCollector;
class PartitionTraverser;
class ShadowCasterCollector;

// Entities
class Entity;
class JointTag;
class Mesh;
class SegmentSet;
class SkyBox;
class TextureProjector;

// Events
class Event;
class GeometryEvent;
class LensEvent;
class LightEvent;
class Scene3DEvent;
class ShadingMethodEvent;

// Filters
class ColorMatrixFilter;
class ColorTransformFilter;
class FilterBase;
class MotionBlurFilter;
class ColorMatrixFilterTask;
class ColorTransformFilterTask;
class DoubleBufferCopyFilterTask;
class FilterTaskBase;
class XFadeCompositeFilterTask;

// Library
class AssetBase;

// Lights
class CubeMapShadowMapper;
class DirectionalShadowMapper;
class ShadowMapperBase;
class DirectionalLight;
class LightBase;
class LightProbe;
class PointLight;

// Materials
class MethodDependencyCounter;
class RegisterPool;
class ShaderCompiler;
class ShaderRegisterCache;
class ShaderRegisterData;
class SuperShaderCompiler;
class LightPickerBase;
class StaticLightPicker;
class BasicAmbientMethod;
class BasicDiffuseMethod;
class BasicNormalMethod;
class BasicSpecularMethod;
class ColorMatrixMethod;
class ColorTransformMethod;
class EffectMethodBase;
class EnvMapMethod;
class FogMethod;
class HardShadowMapMethod;
class LightingMethodBase;
class LightMapMethod;
class MethodVO;
class MethodVOSet;
class OutlineMethod;
class ProjectiveTextureMethod;
class RimLightMethod;
class ShaderMethodSetup;
class ShadingMethodBase;
class ShadowMapMethodBase;
class SimpleShadowMapMethodBase;
class TerrainDiffuseMethod;
class CompiledPass;
class DepthMapPass;
class DistanceMapPass;
class MaterialPassBase;
class OutlinePass;
class SegmentPass;
class SkyBoxPass;
class SuperShaderPass;
class ColorMaterial;
class MaterialBase;
enum class BlendMode;
class SegmentMaterial;
class SinglePassMaterialBase;
class SkyBoxMaterial;
class TextureMaterial;

// Math
class ColorTransform;
class Matrix;
class Matrix3D;
class Plane3D;
class Point;
class Quaternion;
class Rectangle;
class Vector3D;

// Primitives
class Segment;
class CapsuleGeometry;
class ConeGeometry;
class CubeGeometry;
class CylinderGeometry;
class PlaneGeometry;
class PrimitiveBase;
class SphereGeometry;
class TorusGeometry;
class WireframeCube;
class WireframeCylinder;
class WireframePlane;
class WireframePrimitiveBase;
class WireframeSphere;
class WireframeTriangles;

// Textures
class ATFCubeTexture;
class ATFTexture;
class BitmapTexture;
class CubeTextureBase;
class RenderCubeTexture;
class RenderTexture;
class Texture2DBase;
class TextureProxyBase;

// Tools
class ParticleGeometryTransform;

// Utils
class BitmapData;
class ByteArray;

// RenderSystems
class VertexBuffer;
class IndexBuffer;
class Program;
class TextureBase;
class Texture;
class CubeTexture;
class Matrix3D;
class Rectangle;
enum class BlendFactor;
enum class BufferUsage;
enum class ClearMask;
enum class TriangleFace;
enum class CompareMode;
enum class FillMode;
enum class ProgramType;
enum class StencilAction;
enum class TextureFormat;
enum class WrapMode;
enum class TextureFilter;
enum class MipFilter;
enum class VertexBufferFormat;
class IContext;

// Typedefs
typedef std::vector<SubMesh*> SubMeshVector;
typedef std::vector<ISubGeometry*> SubGeometryVector;
typedef std::vector<MaterialPassBase*> MaterialPassVector;
typedef std::vector<Plane3D> PlaneVector;
typedef std::vector<LightBase*> LightVector;
typedef std::vector<PointLight*> PointLightVector;
typedef std::vector<DirectionalLight*> DirectionalLightVector;
typedef std::vector<LightProbe*> LightProbeVector;
typedef std::vector<float> WeightVector;
typedef std::vector<FilterBase*> FilterVector;

AWAY_NAMESPACE_END