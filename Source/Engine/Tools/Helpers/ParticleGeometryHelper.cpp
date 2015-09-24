#include "ParticleGeometryHelper.h"
#include "ParticleGeometryTransform.h"
#include "CompactSubGeometry.h"
#include "ParticleAnimationData.h"
#include "ParticleGeometry.h"
#include "Vector3D.h"
#include "Point.h"
#include "Matrix.h"
#include "Matrix3D.h"

USING_AWAY_NAMESPACE

ParticleGeometry* ParticleGeometryHelper::generateGeometry(std::vector<Geometry*>& geometries, ParticleGeometryTransform* transforms)
{
	std::vector<int> sub2SubMap, vertexCounters;
	std::vector<CompactSubGeometry*> subGeometries;
	std::vector<std::vector<float>*> verticesVector;
	std::vector<std::vector<unsigned short>*> indicesVector;
	ParticleData* particleData;

	int numParticles = geometries.size();
	int numSubGeometries, numVertices, numTriangles;
	int vertexCounter, stride, product, i, j, k;
	ISubGeometry* sourceSubGeometry;
	float* sourceVertices;
	unsigned short* sourceIndices;
	Vector3D position, normal, tangent;
	Point<float> uv;
	ParticleGeometry* particleGeometry = new ParticleGeometry();

	for (i = 0; i < numParticles; i++)
	{
		SubGeometryVector& sourceSubGeometries = geometries[i]->getSubGeometries();
		numSubGeometries = sourceSubGeometries.size();

		for (int srcIndex = 0; srcIndex < numSubGeometries; srcIndex++)
		{
			// create a different particle subgeometry group for each source subgeometry in a particle
			sourceSubGeometry = sourceSubGeometries[srcIndex];
			numVertices = sourceSubGeometry->getVertexCount();
			numTriangles = sourceSubGeometry->getTriangleCount();
			if (sub2SubMap.size() <= srcIndex)
			{
				sub2SubMap.push_back(subGeometries.size());
				verticesVector.push_back(new std::vector<float>());
				indicesVector.push_back(new std::vector<unsigned short>());
				subGeometries.push_back(new CompactSubGeometry());
				vertexCounters.push_back(0);
			}

			// add a new particle subgeometry if this source subgeometry will take us over the 65536 limit
			if (numVertices + vertexCounters[sub2SubMap[srcIndex]] > 65536)
			{
				// update submap and add new subgeom vectors
				sub2SubMap[srcIndex] = subGeometries.size();
				verticesVector.push_back(new std::vector<float>());
				indicesVector.push_back(new std::vector<unsigned short>());
				subGeometries.push_back(new CompactSubGeometry());
				vertexCounters.push_back(0);
			}

			j = sub2SubMap[srcIndex];

			// select the correct vector
			std::vector<float>* vertices = verticesVector[j];
			std::vector<unsigned short>* indices = indicesVector[j];
			vertexCounter = vertexCounters[j];
			vertexCounters[j] += numVertices;

			particleData = new ParticleData();
			particleData->m_numVertices = numVertices;
			particleData->m_startVertexIndex = vertexCounter;
			particleData->m_particleIndex = i;
			particleData->m_subGeometry = subGeometries[j];
			particleGeometry->m_particles.push_back(particleData);

			stride = sourceSubGeometry->getVertexStride();
			sourceVertices = sourceSubGeometry->getVertexData();
			if (transforms)
			{
				ParticleGeometryTransform& particleGeometryTransform = transforms[i];
				Matrix3D* vertexTransform = particleGeometryTransform.getVertexTransform();
				Matrix3D* invVertexTransform = particleGeometryTransform.getInverseVertexTransform();
				Matrix* uvTransform = particleGeometryTransform.getUVTransform();
				for (k = 0; k < numVertices; k++)
				{
					product = k * stride;
					position.m_x = sourceVertices[product];
					position.m_y = sourceVertices[product + 1];
					position.m_z = sourceVertices[product + 2];
					normal.m_x = sourceVertices[product + 3];
					normal.m_y = sourceVertices[product + 4];
					normal.m_z = sourceVertices[product + 5];
					tangent.m_x = sourceVertices[product + 6];
					tangent.m_y = sourceVertices[product + 7];
					tangent.m_z = sourceVertices[product + 8];
					uv.m_x = sourceVertices[product + 9];
					uv.m_y = sourceVertices[product + 10];
					if (vertexTransform)
					{
						Matrix3D::transformVector(*vertexTransform, &position, &position);
						Matrix3D::deltaTransformVector(*invVertexTransform, &normal, &normal);
						Matrix3D::deltaTransformVector(*invVertexTransform, &tangent, &tangent);
					}
					if (uvTransform)
					{
						Matrix::transformPoint(*uvTransform, &uv, &uv);
					}
					vertices->push_back(position.m_x);
					vertices->push_back(position.m_y);
					vertices->push_back(position.m_z);
					vertices->push_back(normal.m_x);
					vertices->push_back(normal.m_y);
					vertices->push_back(normal.m_z);
					vertices->push_back(tangent.m_x);
					vertices->push_back(tangent.m_y);
					vertices->push_back(tangent.m_z);
					vertices->push_back(uv.m_x);
					vertices->push_back(uv.m_y);
					vertices->push_back(uv.m_x);
					vertices->push_back(uv.m_y);
				}
			}
			else
			{
				for (k = 0; k < numVertices; k++)
				{
					product = k * stride;
					vertices->push_back(sourceVertices[product]);
					vertices->push_back(sourceVertices[product + 1]);
					vertices->push_back(sourceVertices[product + 2]);
					vertices->push_back(sourceVertices[product + 3]);
					vertices->push_back(sourceVertices[product + 4]);
					vertices->push_back(sourceVertices[product + 5]);
					vertices->push_back(sourceVertices[product + 6]);
					vertices->push_back(sourceVertices[product + 7]);
					vertices->push_back(sourceVertices[product + 8]);
					vertices->push_back(sourceVertices[product + 9]);
					vertices->push_back(sourceVertices[product + 10]);
					vertices->push_back(sourceVertices[product + 9]);
					vertices->push_back(sourceVertices[product + 10]);
				}
			}

			sourceIndices = sourceSubGeometry->getIndexData();
			for (k = 0; k < numTriangles; k++)
			{
				product = k * 3;
				indices->push_back(sourceIndices[product] + vertexCounter);
				indices->push_back(sourceIndices[product + 1] + vertexCounter);
				indices->push_back(sourceIndices[product + 2] + vertexCounter);
			}
		}
	}

	particleGeometry->m_numParticles = numParticles;
	numParticles = subGeometries.size();
	for (i = 0; i < numParticles; i++)
	{
		CompactSubGeometry* subGeometry = subGeometries[i];
		subGeometry->updateData(verticesVector[i]->data(), verticesVector[i]->size() / subGeometry->getVertexStride());
		subGeometry->updateIndexData(indicesVector[i]->data(), indicesVector[i]->size());
		particleGeometry->addSubGeometry(subGeometry);
	}

	return particleGeometry;
}