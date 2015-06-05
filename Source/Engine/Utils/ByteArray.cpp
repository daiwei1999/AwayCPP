#include "ByteArray.h"

USING_AWAY_NAMESPACE

#define MALLOC_SIZE 1024

ByteArray::ByteArray()
{
	m_capacity = MALLOC_SIZE;
	m_pData = (unsigned char*)std::malloc(m_capacity);
	m_position = m_length = 0;
	m_littleEndian = true;
}

ByteArray::~ByteArray()
{
	std::free(m_pData);
}

void ByteArray::setLength(unsigned int value)
{
	m_length = value;
	m_pData = (unsigned char*)std::realloc(m_pData, m_length);
	m_capacity = value;
}

void ByteArray::checkSpace(unsigned int length)
{
	if (length > m_capacity)
	{
		m_capacity += MALLOC_SIZE;
		m_pData = (unsigned char*)std::realloc(m_pData, m_capacity);
		m_length = length;
	}
}

bool ByteArray::readBoolean()
{
	bool value;
	std::memcpy(&value, m_pData + m_position, 1);
	m_position += 1;
	return value;
}

char ByteArray::readByte()
{
	char value;
	std::memcpy(&value, m_pData + m_position, 1);
	m_position += 1;
	return value;
}

unsigned char ByteArray::readUnsignedByte()
{
	unsigned char value;
	std::memcpy(&value, m_pData + m_position, 1);
	m_position += 1;
	return value;
}

short ByteArray::readShort()
{
	short value;
	if (m_littleEndian)
	{
		std::memcpy(&value, m_pData + m_position, 2);
		m_position += 2;
	}
	else
	{
		unsigned char ch1 = readUnsignedByte();
		unsigned char ch2 = readUnsignedByte();
		value = (ch1 << 8) | ch2;
	}
	return value;
}

unsigned short ByteArray::readUnsignedShort()
{
	unsigned short value;
	if (m_littleEndian)
	{
		std::memcpy(&value, m_pData + m_position, 2);
		m_position += 2;
	}
	else
	{
		unsigned char ch1 = readUnsignedByte();
		unsigned char ch2 = readUnsignedByte();
		value = (ch1 << 8) | ch2;
	}
	return value;
}

int ByteArray::readInt()
{
	int value;
	if (m_littleEndian)
	{
		std::memcpy(&value, m_pData + m_position, 4);
		m_position += 4;
	}
	else
	{
		unsigned char ch1 = readUnsignedByte();
		unsigned char ch2 = readUnsignedByte();
		unsigned char ch3 = readUnsignedByte();
		unsigned char ch4 = readUnsignedByte();
		value = (ch1 << 24) | (ch2 << 16) | (ch3 << 8) | ch4;
	}
	return value;
}

unsigned int ByteArray::readUnsignedInt()
{
	unsigned int value;
	if (m_littleEndian)
	{
		std::memcpy(&value, m_pData + m_position, 4);
		m_position += 4;
	}
	else
	{
		unsigned char ch1 = readUnsignedByte();
		unsigned char ch2 = readUnsignedByte();
		unsigned char ch3 = readUnsignedByte();
		unsigned char ch4 = readUnsignedByte();
		value = (ch1 << 24) | (ch2 << 16) | (ch3 << 8) | ch4;
	}
	return value;
}

float ByteArray::readFloat()
{
	float value;
	std::memcpy(&value, m_pData + m_position, 4);
	m_position += 4;
	return value;
}

double ByteArray::readDouble()
{
	double value;
	std::memcpy(&value, m_pData + m_position, 8);
	m_position += 8;
	return value;
}

unsigned char ByteArray::readString(char* str)
{
	unsigned char length = readUnsignedByte();
	std::memcpy(str, m_pData + m_position, length);
	m_position += length;
	return length;
}

void ByteArray::readBlock(void* dest, unsigned int count)
{
	std::memcpy(dest, m_pData + m_position, count);
	m_position += count;
}

void ByteArray::writeBoolean(bool value)
{
	writeByte(value ? 1 : 0);
}

void ByteArray::writeByte(char value)
{
	checkSpace(m_position + 1);
	std::memcpy(m_pData + m_position, &value, 1);
	m_position += 1;
}

void ByteArray::writeShort(short value)
{
	checkSpace(m_position + 2);
	std::memcpy(m_pData + m_position, &value, 2);
	m_position += 2;
}

void ByteArray::writeInt(int value)
{
	checkSpace(m_position + 4);
	std::memcpy(m_pData + m_position, &value, 4);
	m_position += 4;
}

void ByteArray::writeFloat(float value)
{
	checkSpace(m_position + 4);
	std::memcpy(m_pData + m_position, &value, 4);
	m_position += 4;
}

void ByteArray::writeDouble(double value)
{
	checkSpace(m_position + 8);
	std::memcpy(m_pData + m_position, &value, 8);
	m_position += 8;
}

void ByteArray::writeBytes(unsigned char* data, unsigned int offset, unsigned int length)
{
	checkSpace(m_position + length);
	std::memcpy(m_pData + m_position, data + offset, length);
	m_position += length;
}