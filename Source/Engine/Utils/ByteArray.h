#pragma once

#include "Common.h"

AWAY_NAMESPACE_BEGIN

enum class Endian
{
	BIG,
	LITTLE
};

class ByteArray
{
public:
	ByteArray();
	~ByteArray();

public:
	bool readBoolean();
	char readByte();
	unsigned char readUnsignedByte();
	short readShort();
	unsigned short readUnsignedShort();
	int readInt();
	unsigned int readUnsignedInt();
	float readFloat();
	double readDouble();
	unsigned char readString(char* str);
	void readBlock(void* dest, unsigned int count);

	void writeBoolean(bool value);
	void writeByte(char value);
	void writeShort(short value);
	void writeInt(int value);
	void writeFloat(float value);
	void writeDouble(double value);
	void writeBytes(unsigned char* data, unsigned int offset, unsigned int length);

	Endian getEndian() { return m_littleEndian ? Endian::LITTLE : Endian::BIG; }
	void setEndian(Endian value) { m_littleEndian = (value == Endian::LITTLE); }

	unsigned int getLength() { return m_length; }
	void setLength(unsigned int value);

	unsigned int getPosition() { return m_position; }
	void setPosition(unsigned int value) { m_position = value; }

	unsigned char operator[](int index) { return m_pData[index]; }
	unsigned char* getByteData() { return m_pData + m_position; }

private:
	void checkSpace(unsigned int length);

private:
	unsigned char* m_pData;
	unsigned int m_capacity;
	unsigned int m_position;
	unsigned int m_length;
	bool m_littleEndian;
};

AWAY_NAMESPACE_END