#pragma once

#include "Common.h"
#include "CompactSubGeometry.h"

AWAY_NAMESPACE_BEGIN

/**
 * SkinnedSubGeometry provides a SubGeometry extension that contains data needed to skin vertices. In particular,
 * it provides joint indices and weights.
 * Important! Joint indices need to be pre-multiplied by 3, since they index the matrix array (and each matrix has 3 float4 elements)
 */
class SkinnedSubGeometry : public CompactSubGeometry
{
public:
	SkinnedSubGeometry(unsigned char jointsPerVertex);

	unsigned char* getJointIndexData() { return m_jointIndexData; }
	float* getJointWeightsData() { return m_jointWeightsData; }
	void activateJointIndexBuffer(int index, IContext* context);
	void activateJointWeightsBuffer(int index, IContext* context);
	void updateJointIndexData(unsigned char* data);
	void updateJointWeightsData(float* data);
	void updateAnimatedData(float* data);

protected:
	void uploadData() override;

private:
	VertexBufferFormat m_jointIndexFormat;
	VertexBufferFormat m_jointWeightFormat;
	unsigned char* m_jointIndexData;
	float* m_jointWeightsData;
	float* m_animatedData = nullptr; // used for cpu skinning
	unsigned char m_jointsPerVertex;
	bool m_jointWeightsInvalid;
	bool m_jointIndicesInvalid;
	IContext* m_jointWeightContext = nullptr;
	IContext* m_jointIndexContext = nullptr;
	VertexBuffer* m_jointWeightsBuffer = nullptr;
	VertexBuffer* m_jointIndexBuffer = nullptr;
};

AWAY_NAMESPACE_END