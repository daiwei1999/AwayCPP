#include "ShaderChunk.h"

USING_AWAY_NAMESPACE

ShaderChunk::ShaderChunk(unsigned int version, bool hasHeader)
{
	if (version == 1)
		m_capacity = 200 * 24;
	else if (version == 2)
		m_capacity = 1024 * 24;
	else
		m_capacity = 2048 * 24;

	if (hasHeader)
		m_capacity += 7;

	m_pBytes = (unsigned char*)std::malloc(m_capacity);
	m_position = 0;
}

ShaderChunk::~ShaderChunk()
{
	std::free(m_pBytes);
}

void ShaderChunk::writeOp000()
{
	std::memset(m_pBytes + m_position, 0, 20);
	m_position += 20;
}

void ShaderChunk::writeOp010(unsigned int src)
{
	// dest
	std::memset(m_pBytes + m_position, 0, 4);
	m_position += 4;

	// src1
	if ((src & 0x10000) == 0)
		writeSourceD(src);
	else
		writeSourceI(src);

	// src2
	std::memset(m_pBytes + m_position, 0, 8);
	m_position += 8;
}

void ShaderChunk::writeOp011(unsigned int src1, unsigned int src2)
{
	// dest
	std::memset(m_pBytes + m_position, 0, 4);
	m_position += 4;

	// src1
	if ((src1 & 0x10000) == 0)
		writeSourceD(src1);
	else
		writeSourceI(src1);

	// src2
	if ((src2 & 0x10000) == 0)
		writeSourceD(src2);
	else
		writeSourceI(src2);
}

void ShaderChunk::writeOp110(unsigned int dest, unsigned int src)
{
	// dest
	writeDest(dest);

	// src1
	if ((src & 0x10000) == 0)
		writeSourceD(src);
	else
		writeSourceI(src);

	// src2
	std::memset(m_pBytes + m_position, 0, 8);
	m_position += 8;
}

void ShaderChunk::writeOp111(unsigned int dest, unsigned int src1, unsigned int src2)
{
	// dest
	writeDest(dest);

	// src1
	if ((src1 & 0x10000) == 0)
		writeSourceD(src1);
	else
		writeSourceI(src1);

	// src2
	if ((src2 & 0x10000) == 0)
		writeSourceD(src2);
	else
		writeSourceI(src2);
}

void ShaderChunk::writeOpTex(unsigned int dest, unsigned int tcoord, unsigned int sampler)
{
	// dest
	writeDest(dest);

	// src1
	if ((tcoord & 0x10000) == 0)
		writeSourceD(tcoord);
	else
		writeSourceI(tcoord);

	// src2
	writeUnsignedInt((sampler & 0x1F) | ((sampler & 0x1FE0) << 11));
	writeUnsignedInt(
        ((sampler & 0xE0000000) >> 1) | 
        ((sampler & 0x1C000000) >> 2) | 
        ((sampler & 0x3F80000) >> 3) | 
        ((sampler & 0x70000) >> 4) | 
        ((sampler & 0xE000) >> 5) | 
        5);
}

void ShaderChunk::writeSourceD(unsigned int src)
{
	writeUnsignedInt(src & 0xFF000FFF);
	writeUnsignedInt((src >> 12) & 0xF);
}

void ShaderChunk::writeSourceI(unsigned int src)
{
	writeUnsignedInt((src & 0xFF000000) | ((src & 0x3F) << 16) | ((src >> 17) & 0x7F));
	writeUnsignedInt((src & 0xF00) | ((src >> 12) & 0xF) | ((src << 10) & 0x30000) | 0x80000000);
}

void ShaderChunk::writeDest(unsigned int dest)
{
	writeUnsignedInt(((
		(1 << ((dest >> 24) & 3)) | 
		(1 << ((dest >> 26) & 3)) | 
		(1 << ((dest >> 28) & 3)) | 
		(1 << (dest >> 30))
		) << 16) | (dest & 0x0FFF) | ((dest & 0xF000) << 12));
}

void ShaderChunk::mov(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x00);
	writeOp110(dest, src);
}

void ShaderChunk::add(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x01);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::sub(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x02);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::mul(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x03);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::div(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x04);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::rcp(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x05);
	writeOp110(dest, src);
}

void ShaderChunk::min(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x06);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::max(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x07);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::frc(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x08);
	writeOp110(dest, src);
}

void ShaderChunk::sqt(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x09);
	writeOp110(dest, src);
}

void ShaderChunk::rsq(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x0a);
	writeOp110(dest, src);
}

void ShaderChunk::pow(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x0b);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::log(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x0c);
	writeOp110(dest, src);
}

void ShaderChunk::exp(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x0d);
	writeOp110(dest, src);
}

void ShaderChunk::nrm(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x0e);
	writeOp110(dest, src);
}

void ShaderChunk::sin(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x0f);
	writeOp110(dest, src);
}

void ShaderChunk::cos(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x10);
	writeOp110(dest, src);
}

void ShaderChunk::crs(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x11);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::dp3(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x12);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::dp4(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x13);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::abs(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x14);
	writeOp110(dest, src);
}

void ShaderChunk::neg(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x15);
	writeOp110(dest, src);
}

void ShaderChunk::sat(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x16);
	writeOp110(dest, src);
}

void ShaderChunk::m33(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x17);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::m44(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x18);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::m34(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x19);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::ddx(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x1a);
	writeOp110(dest, src);
}

void ShaderChunk::ddy(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x1b);
	writeOp110(dest, src);
}

void ShaderChunk::ife(unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x1c);
	writeOp011(src1, src2);
}

void ShaderChunk::ine(unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x1d);
	writeOp011(src1, src2);
}

void ShaderChunk::ifg(unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x1e);
	writeOp011(src1, src2);
}

void ShaderChunk::ifl(unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x1f);
	writeOp011(src1, src2);
}

void ShaderChunk::els()
{
	writeUnsignedInt(0x20);
	writeOp000();
}

void ShaderChunk::eif()
{
	writeUnsignedInt(0x21);
	writeOp000();
}

void ShaderChunk::ted(unsigned int dest, unsigned int tcoord, unsigned int sampler)
{
	writeUnsignedInt(0x26);
	writeOpTex(dest, tcoord, sampler);
}

void ShaderChunk::kil(unsigned int src)
{
	writeUnsignedInt(0x27);
	writeOp010(src);
}

void ShaderChunk::tex(unsigned int dest, unsigned int tcoord, unsigned int sampler)
{
	writeUnsignedInt(0x28);
	writeOpTex(dest, tcoord, sampler);
}

void ShaderChunk::sge(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x29);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::slt(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x2a);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::sgn(unsigned int dest, unsigned int src)
{
	writeUnsignedInt(0x2b);
	writeOp110(dest, src);
}

void ShaderChunk::seq(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x2c);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::sne(unsigned int dest, unsigned int src1, unsigned int src2)
{
	writeUnsignedInt(0x2d);
	writeOp111(dest, src1, src2);
}

void ShaderChunk::append(ShaderChunk* chunk)
{
	std::memcpy(m_pBytes + m_position, chunk->m_pBytes, chunk->m_position);
	m_position += chunk->m_position;
}