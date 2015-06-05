#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class CRC32
{
public:
	static void reset() { m_crc = 0; };
	static void update(ByteArray& bytes);
	static unsigned int getValue() { return m_crc; }

private:
	static unsigned int m_crc;
	static const unsigned int m_crcTable[256];
};

AWAY_NAMESPACE_END