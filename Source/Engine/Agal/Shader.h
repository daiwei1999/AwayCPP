#pragma once

#include "Common.h"
#include "ShaderChunk.h"

AWAY_NAMESPACE_BEGIN

class Shader : public ShaderChunk
{
public:
	Shader(ProgramType type, unsigned int version = 1);

	void prepend(ShaderChunk* chunk);

public:
	void writeBytes(ByteArray& bytes);
};

AWAY_NAMESPACE_END