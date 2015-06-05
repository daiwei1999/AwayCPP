#include "RegisterPool.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

std::vector<unsigned int> RegisterPool::m_regPool[6];
std::vector<unsigned int> RegisterPool::m_regCompsPool[6][4];

RegisterPool::RegisterPool(RegisterType type, int regCount, bool persistent)
{
	m_type = static_cast<int>(type);
	m_regCount = regCount;
	m_persistent = persistent;
	initRegisters(m_type, regCount);
	m_usedVectorCount.resize(regCount);
	for (int i = 0; i < 4; i++)
		m_usedSingleCount[i].resize(regCount);
}

void RegisterPool::reset()
{
	for (int i = 0; i < m_regCount; i++)
	{
		m_usedVectorCount[i] = 0;
		for (int j = 0; j < 4; j++)
			m_usedSingleCount[j][i] = 0;
	}
}

unsigned int RegisterPool::requestFreeVectorReg()
{
	for (int i = 0; i < m_regCount; i++)
	{
		if (!isRegisterUsed(i))
		{
			if (m_persistent)
				m_usedVectorCount[i]++;

			return m_regPool[m_type][i];
		}
	}

	// Register overflow!
	assert(false);
	return 0;
}

unsigned int RegisterPool::requestFreeRegComponent()
{
	for (int i = 0; i < m_regCount; i++)
	{
		if (m_usedVectorCount[i] > 0)
			continue;

		for (int j = 0; j < 4; j++)
		{
			if (m_usedSingleCount[j][i] == 0)
			{
				if (m_persistent)
					m_usedSingleCount[j][i]++;

				return m_regCompsPool[m_type][j][i];
			}
		}
	}

	// Register overflow!
	assert(false);
	return 0;
}

void RegisterPool::addUsage(unsigned int value, int usageCount)
{
	unsigned int index = value & 0xFFF;
	if (value >> 24 != 0xE4)
		m_usedSingleCount[value >> 30][index] += usageCount;
	else
		m_usedVectorCount[index] += usageCount;
}

void RegisterPool::removeUsage(unsigned int value)
{
	unsigned int index = value & 0xFFF;
	if (value >> 24 != 0xE4)
	{
		if (--m_usedSingleCount[value >> 30][index] < 0)
			assert(false); // More usages removed than exist!
	}
	else
	{
		if (--m_usedVectorCount[index] < 0)
			assert(false); // More usages removed than exist!
	}
}

bool RegisterPool::hasRegisteredRegs()
{
	for (int i = 0; i < m_regCount; i++)
	{
		if (isRegisterUsed(i))
			return true;
	}
	return false;
}

bool RegisterPool::isRegisterUsed(int index)
{
	if (m_usedVectorCount[index] > 0)
		return true;

	for (int i = 0; i < 4; i++)
	{
		if (m_usedSingleCount[i][index] > 0)
			return true;
	}
	return false;
}

unsigned int RegisterPool::getRegisterValue(int type, unsigned int index, int component)
{
	unsigned int value = index;
	if (type != 5)
		value += (type << 12) + 0xE4000000;

	if (component > -1)
	{
		unsigned int components[4] = { Regs::x, Regs::y, Regs::z, Regs::w };
		value ^= components[component];
	}

	return value;
}

void RegisterPool::initRegisters(int type, int regCount)
{
	std::vector<unsigned int>& vectorRegisters = m_regPool[type];
	if (vectorRegisters.size() == 0)
	{
		std::vector<unsigned int>(&registerComponents)[4] = m_regCompsPool[type];

		for (int i = 0; i < regCount; i++)
		{
			vectorRegisters.push_back(getRegisterValue(type, i));

			for (int j = 0; j < 4; j++)
				registerComponents[j].push_back(getRegisterValue(type, i, j));
		}
	}
}