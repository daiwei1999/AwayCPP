#include "Shader.h"
#include "ByteArray.h"
#include "IContext.h"

USING_AWAY_NAMESPACE

Shader::Shader(ProgramType type, unsigned int version) : ShaderChunk(version, true)
{
	m_pBytes[m_position++] = 0xA0;
	writeUnsignedInt(version);
	m_pBytes[m_position++] = 0xA1;
	m_pBytes[m_position++] = (type == ProgramType::VERTEX) ? 0 : 1;
}

void Shader::writeBytes(ByteArray& bytes)
{
	if (m_position > m_capacity)
		assert(false);

	bytes.setLength(m_position);
	bytes.setPosition(0);
	bytes.writeBytes(m_pBytes, 0, m_position);
}

void Shader::prepend(ShaderChunk* chunk)
{
	unsigned char* src = m_pBytes + 7;
	std::memmove(src + chunk->m_position, src, m_position - 7);
	std::memcpy(src, chunk->m_pBytes, chunk->m_position);
	m_position += chunk->m_position;
}