#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

class ShaderChunk
{
public:
	ShaderChunk(unsigned int version = 1, bool hasHeader = false);
	~ShaderChunk();

public:
	void mov(unsigned int dest, unsigned int src); // 0x00
	void add(unsigned int dest, unsigned int src1, unsigned int src2); // 0x01
	void sub(unsigned int dest, unsigned int src1, unsigned int src2); // 0x02
	void mul(unsigned int dest, unsigned int src1, unsigned int src2); // 0x03
	void div(unsigned int dest, unsigned int src1, unsigned int src2); // 0x04
	void rcp(unsigned int dest, unsigned int src); // 0x05
	void min(unsigned int dest, unsigned int src1, unsigned int src2); // 0x06
	void max(unsigned int dest, unsigned int src1, unsigned int src2); // 0x07
	void frc(unsigned int dest, unsigned int src); // 0x08
	void sqt(unsigned int dest, unsigned int src); // 0x09
	void rsq(unsigned int dest, unsigned int src); // 0x0a
	void pow(unsigned int dest, unsigned int src1, unsigned int src2); // 0x0b
	void log(unsigned int dest, unsigned int src); // 0x0c
	void exp(unsigned int dest, unsigned int src); // 0x0d
	void nrm(unsigned int dest, unsigned int src); // 0x0e
	void sin(unsigned int dest, unsigned int src); // 0x0f
	void cos(unsigned int dest, unsigned int src); // 0x10
	void crs(unsigned int dest, unsigned int src1, unsigned int src2); // 0x11
	void dp3(unsigned int dest, unsigned int src1, unsigned int src2); // 0x12
	void dp4(unsigned int dest, unsigned int src1, unsigned int src2); // 0x13
	void abs(unsigned int dest, unsigned int src); // 0x14
	void neg(unsigned int dest, unsigned int src); // 0x15
	void sat(unsigned int dest, unsigned int src); // 0x16
	void m33(unsigned int dest, unsigned int src1, unsigned int src2); // 0x17
	void m44(unsigned int dest, unsigned int src1, unsigned int src2); // 0x18
	void m34(unsigned int dest, unsigned int src1, unsigned int src2); // 0x19
	void ddx(unsigned int dest, unsigned int src); // 0x1a
	void ddy(unsigned int dest, unsigned int src); // 0x1b
	void ife(unsigned int dest, unsigned int src); // 0x1c
	void ine(unsigned int dest, unsigned int src); // 0x1d
	void ifg(unsigned int dest, unsigned int src); // 0x1e
	void ifl(unsigned int dest, unsigned int src); // 0x1f
	void els(); // 0x20
	void eif(); // 0x21
	void ted(unsigned int dest, unsigned int src1, unsigned int src2); // 0x26
	void kil(unsigned int src); // 0x27
	void tex(unsigned int dest, unsigned int src1, unsigned int src2); // 0x28
	void sge(unsigned int dest, unsigned int src1, unsigned int src2); // 0x29
	void slt(unsigned int dest, unsigned int src1, unsigned int src2); // 0x2a
	void sgn(unsigned int dest, unsigned int src); // 0x2b
	void seq(unsigned int dest, unsigned int src1, unsigned int src2); // 0x2c
	void sne(unsigned int dest, unsigned int src1, unsigned int src2); // 0x2d

	void append(ShaderChunk* chunk);

protected:
	void writeUnsignedInt(unsigned int value)
	{
		std::memcpy(m_pBytes + m_position, &value, 4);
		m_position += 4;
	}

private:
	void writeOp0();
	void writeOp1(unsigned int src);
	void writeOp2(unsigned int dest, unsigned int src);
	void writeOp3(unsigned int dest, unsigned int src1, unsigned int src2);
	void writeOpTex(unsigned int dest, unsigned int tcoord, unsigned int sampler);
	void writeSourceD(unsigned int src);
	void writeSourceI(unsigned int src);
	void writeDest(unsigned int dest);

	friend class Shader;
protected:
	unsigned char* m_pBytes;
	unsigned int m_position;
	unsigned int m_capacity;
};

AWAY_NAMESPACE_END