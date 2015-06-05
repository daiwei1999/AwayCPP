#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class Destination
{
public:
	char m_indexOffset; // Indirect offset (8 bits)
	unsigned char m_swizzle; //  Swizzle (8 bits, 2 bits per component)
	unsigned char m_regType; // Register type(4 bits)
	unsigned char m_mask; //  Write mask(4 bits)
	unsigned short m_regNum; // Register number(16 bits)
	unsigned char m_indexRegType; // Index register type (4 bits)
	unsigned char m_indexSelect; // Index register component select (2 bits)
	unsigned char m_indirectFlag; //  0=Direct,1=Indirect (for direct m_indexRegType and m_indexSelect are ignored)
	unsigned char m_dim; // Sampler Dimension(0=2D, 1=Cube)
};

class Token
{
public:
	unsigned int m_opcode;
	Destination m_dest;
	Destination m_src1;
	Destination m_src2;
};

class Description
{
public:
	Description()
	{
		m_hasIndirect = false;
		m_hasMatrix = false;
	}

public:
	unsigned int m_version; // agal version
	unsigned char m_shaderType; // 0 for a vertex program; 1 for a fragment program
	bool m_hasIndirect;
	//bool m_writeDepth;
	bool m_hasMatrix;

	std::vector<Token> m_tokens;
	std::map<unsigned short, unsigned char> m_regRead[5];
	std::map<unsigned short, unsigned char> m_regWrite[5];
	std::map<unsigned short, unsigned char> m_samplers;
};

class AGLSLParser
{
public:
	static std::string parse(ByteArray& bytes, bool& hasIndirect);

private:
	static bool parseAGALByteArray(Description& desc, ByteArray& bytes);
	static void readReg(Destination& s, unsigned char mh, Description& desc, ByteArray& bytes);
	static std::string regToString(unsigned int regType, unsigned int regNum, bool hasIndirect, char tag);
	static std::string sourceToString(Destination& s, int subline, unsigned int dwm, bool isScalar, bool hasIndirect, char tag);
};

AWAY_NAMESPACE_END