#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

enum class RegisterType
{
	A, // Attribute Registers
	C, // Constant Registers
	T, // Temporary Registers
	O, // Output Registers
	V, // Varying Registers
	S  // Texture Samplers
};

/**
 * RegisterPool is used by the shader compilation process to keep track of which registers of a certain type are
 * currently used and should not be allowed to be written to. Either entire registers can be requested and locked,
 * or single components (x, y, z, w) of a single register.
 * It is used by ShaderRegisterCache to track usages of individual register types.
 */
class RegisterPool
{
public:
	RegisterPool(RegisterType type, int regCount, bool persistent = true);

	/**
	 * Reset RegisterPool to unused state.
	 */
	void reset();

	/**
	 * Retrieve an entire vector register that's still available.
	 */
	unsigned int requestFreeVectorReg();

	/**
	 * Retrieve a single vector component that's still available.
	 */
	unsigned int requestFreeRegComponent();

	/**
	 * Marks a register as used, so it cannot be retrieved. The register won't be able to be used until removeUsage
	 * has been called usageCount times again.
	 */
	void addUsage(unsigned int value, int usageCount);

	/**
	 * Removes a usage from a register. When usages reach 0, the register is freed again.
	 * @param register The register for which to remove a usage.
	 */
	void removeUsage(unsigned int value);

	/**
	 * Indicates whether or not any registers are in use.
	 */
	bool hasRegisteredRegs();

private:
	bool isRegisterUsed(int index);
	static unsigned int getRegisterValue(int type, unsigned int index, int component = -1);
	static void initRegisters(int type, int regCount);

private:
	static std::vector<unsigned int> m_regPool[6];
	static std::vector<unsigned int> m_regCompsPool[6][4];

	std::vector<int> m_usedSingleCount[4];
	std::vector<int> m_usedVectorCount;
	int m_type;
	int m_regCount;
	bool m_persistent;
};

AWAY_NAMESPACE_END