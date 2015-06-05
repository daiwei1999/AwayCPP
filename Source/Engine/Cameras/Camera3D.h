#pragma once

#include "Common.h"
#include "Entity.h"

AWAY_NAMESPACE_BEGIN

/**
 * A Camera3D object represents a virtual camera through which we view the scene.
 */
class Camera3D : public Entity
{
	DEFINE_CALLBACK_CLASS(Camera3D, onLensMatrixChanged)

public:
	Camera3D(LensBase* lens = nullptr);

	LensBase* getLens() { return m_lens; }
	void setLens(LensBase* value);

	Matrix3D& getViewProjection();
	PlaneVector* getFrustumPlanes();

	void project(Vector3D* point3d, Vector3D* result);
	void unproject(float nX, float nY, float sZ, Vector3D* result);
	void getRay(float nX, float nY, float sZ, Vector3D* result);

protected:
	EntityNode* createEntityPartitionNode() override;
	void updateBounds() override;
	void invalidateSceneTransform() override;

private:
	void updateFrustum();
	void onLensMatrixChanged(Event* event);

private:
	Matrix3D m_viewProjection;
	LensBase* m_lens;
	PlaneVector m_frustumPlanes;
	bool m_viewProjectionDirty;
	bool m_frustumPlanesDirty;
};

AWAY_NAMESPACE_END