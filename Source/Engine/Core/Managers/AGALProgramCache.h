#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class AGALProgramCache
{
public:
	static AGALProgramCache* getInstance(IContext* context);
	static void freeProgram(int programId);

	void setProgram(MaterialPassBase* pass, Shader& vertexShader, Shader& fragmentShader);

private:
	AGALProgramCache(IContext* context);
	void destroyProgram(unsigned int key);
	void freeProgram3D(int programId);

private:
	static unsigned int m_currentId;
	static AGALProgramCache* m_instance;

	IContext* m_context;
	std::map<unsigned int, Program*> m_programs;
	std::map<unsigned int, int> m_ids;
	std::map<unsigned int, unsigned int> m_usages;
	std::map<unsigned int, unsigned int> m_keys;
};

AWAY_NAMESPACE_END