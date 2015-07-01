#include "SkeletonAnimationSet.h"
#include "MaterialPassBase.h"
#include "ShaderChunk.h"
#include "Regs.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

SkeletonAnimationSet::SkeletonAnimationSet(unsigned char jointsPerVertex)
{
	m_jointsPerVertex = jointsPerVertex;
}

void SkeletonAnimationSet::getAGALVertexCode(ShaderChunk& code, MaterialPassBase* pass, std::vector<unsigned int>& sourceRegisters, std::vector<unsigned int>& targetRegisters, unsigned int uvSource, unsigned int uvTarget)
{
	int indexOffset = pass->getUsedVertexConstantCount();
	unsigned int indexStream = Regs::a0 + pass->getUsedStreamCount();
	unsigned int weightStream = indexStream + 1;
	unsigned int indices[4] = { indexStream ^ Regs::x, indexStream ^ Regs::y, indexStream ^ Regs::z, indexStream ^ Regs::w };
	unsigned int weights[4] = { weightStream ^ Regs::x, weightStream ^ Regs::y, weightStream ^ Regs::z, weightStream ^ Regs::w };
	unsigned int temp1 = findTempReg(targetRegisters, {});
	unsigned int temp2 = findTempReg(targetRegisters, { temp1 });

	int len = sourceRegisters.size();
	for (int i = 0; i < len; i++)
	{
		unsigned int src = sourceRegisters[i];
		for (unsigned char j = 0; j < m_jointsPerVertex; j++)
		{
			unsigned int matrixReg = Regs::c(indices[j]) + indexOffset;
			if (i == 0)
			{
				code.dp4(temp1 ^ Regs::x, src, matrixReg);
				code.dp4(temp1 ^ Regs::y, src, matrixReg + 1);
				code.dp4(temp1 ^ Regs::z, src, matrixReg + 2);
			}
			else
			{
				code.dp3(temp1 ^ Regs::x, src, matrixReg);
				code.dp3(temp1 ^ Regs::y, src, matrixReg + 1);
				code.dp3(temp1 ^ Regs::z, src, matrixReg + 2);
			}

			code.mov(temp1 ^ Regs::w, src ^ Regs::w);
			code.mul(temp1, temp1, weights[j]);

			// add or mov to target. Need to write to a temp reg first, because an output can be a target
			if (j == 0)
				code.mov(temp2, temp1);
			else
				code.add(temp2, temp2, temp1);
		}

		code.mov(targetRegisters[i], temp2);
	}
}

void SkeletonAnimationSet::getAGALFragmentCode(ShaderChunk& code, MaterialPassBase* pass, unsigned int shadedTarget)
{

}

void SkeletonAnimationSet::getAGALUVCode(ShaderChunk& code, MaterialPassBase* pass, unsigned int uvSource, unsigned int uvTarget)
{
	code.mov(uvTarget, uvSource);
}

void SkeletonAnimationSet::doneAGALCode(MaterialPassBase* pass)
{

}

void SkeletonAnimationSet::activate(IContext* context, MaterialPassBase* pass)
{

}

void SkeletonAnimationSet::deactivate(IContext* context, MaterialPassBase* pass)
{
	int streamOffset = pass->getUsedStreamCount();
	context->setVertexBufferAt(streamOffset, nullptr);
	context->setVertexBufferAt(streamOffset + 1, nullptr);
}