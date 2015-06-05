#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class Program
{
public:
	Program();
	~Program();

	bool hasVertexIndirect() { return m_hasVertexIndirect; }
	bool hasFragmentIndirect() { return m_hasFragmentIndirect; }
	unsigned int getId() { return m_program; }
	void upload(ByteArray& vertexProgram, ByteArray& fragmentProgram);

	int getUniformLocation(const std::string& name);
	int getAttributeLocation(const std::string& name);

private:
	bool m_hasVertexIndirect;
	bool m_hasFragmentIndirect;
	unsigned int m_program;
	unsigned int m_vertexShader;
	unsigned int m_fragmentShader;
	std::unordered_map<std::string, int> m_uniforms;
	std::unordered_map<std::string, int> m_attributes;
};

AWAY_NAMESPACE_END