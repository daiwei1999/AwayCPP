#include "AGALProgramCache.h"
#include "Program.h"
#include "IContext.h"
#include "Shader.h"
#include "CRC32.h"
#include "ByteArray.h"
#include "MaterialPassBase.h"

USING_AWAY_NAMESPACE

unsigned int AGALProgramCache::m_currentId = 0;
AGALProgramCache* AGALProgramCache::m_instance = nullptr;

AGALProgramCache::AGALProgramCache(IContext* context)
{
	m_context = context;
}

AGALProgramCache* AGALProgramCache::getInstance(IContext* context)
{
	if (!m_instance)
		m_instance = new AGALProgramCache(context);

	return m_instance;
}

void AGALProgramCache::freeProgram(int programId)
{
	if (m_instance)
		m_instance->freeProgram(programId);
}

void AGALProgramCache::setProgram(MaterialPassBase* pass, Shader& vertexShader, Shader& fragmentShader)
{
	ByteArray vertexProgram, fragmentProgram;
	vertexShader.writeBytes(vertexProgram);
	fragmentShader.writeBytes(fragmentProgram);

	CRC32::reset();
	CRC32::update(vertexProgram);
	CRC32::update(fragmentProgram);
	unsigned int key = CRC32::getValue();

	Program* program;
	if (m_programs.find(key) != m_programs.end())
	{
		program = m_programs[key];
	}
	else
	{
		m_keys[m_currentId] = key;
		m_usages[m_currentId] = 0;
		m_ids[key] = m_currentId++;
		program = m_context->createProgram();
		program->upload(vertexProgram, fragmentProgram);
		m_programs[key] = program;
	}

	int oldId = pass->m_programId;
	int newId = m_ids[key];
	if (oldId != newId)
	{
		if (oldId >= 0)
			freeProgram3D(oldId);

		m_usages[newId]++;
	}
	pass->m_programId = newId;
	pass->m_program = program;
}

void AGALProgramCache::destroyProgram(unsigned int key)
{
	delete m_programs[key];
	m_programs.erase(key);
	m_ids[key] = -1;
}

void AGALProgramCache::freeProgram3D(int programId)
{
	m_usages[programId]--;
	if (m_usages[programId] == 0)
		destroyProgram(m_keys[programId]);
}