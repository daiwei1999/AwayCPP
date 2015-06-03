#include "AGLSLParser.h"
#include "Mapping.h"
#include "ByteArray.h"

USING_AWAY_NAMESPACE

std::string AGLSLParser::parse(ByteArray& bytes, bool& hasIndirect)
{
	// parse ByteArray
	Description desc;
	parseAGALByteArray(desc, bytes);

	// generate GLSL string
	std::ostringstream oss;
	oss << "precision highp float;\n";
	oss << "uniform float yflip;\n";
	oss << "vec4 intt0;\n";

	char tag = (desc.m_shaderType == 0) ? 'v' : 'f';
	if (tag == 'v')
		oss << "vec4 outpos;\n";

	// declare uniforms
	hasIndirect = desc.m_hasIndirect;
	if (hasIndirect)
		oss << "uniform vec4 " << tag << "carrr[128];\n";
	else
	{
		for (auto& item : desc.m_regRead[1])
			oss << "uniform vec4 " << tag << "c" << item.first << ";\n";
	}

	// declare temps
	for (auto& item : desc.m_regWrite[2])
		oss << "vec4 " << tag << "t" << item.first << ";\n";

	// declare attributes
	for (auto& item : desc.m_regRead[0])
		oss << "attribute vec4 va" << item.first << ";\n";

	// declare varyings
	for (auto& item : desc.m_regRead[4])
		oss << "varying vec4 v" << item.first << ";\n";

	for (auto& item : desc.m_regWrite[4])
		oss << "varying vec4 v" << item.first << ";\n";

	// declare samplers
	char sampleTypes[2][5] = { "2D", "Cube" };
	for (auto& item : desc.m_samplers)
	{
		unsigned char byte = item.second;
		oss << "uniform sampler" << sampleTypes[byte >> 4] << " fs" << item.first << ";\n";
#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
		if ((byte & 0xf) == 2)
			oss << "uniform sampler" << sampleTypes[byte >> 4] << " fw" << item.first << ";\n";
#endif
	}

	// start body of code
	oss << "void main(){\n";

	std::string originalDestStr, destRegStr;
	char swizzles[4] = { 'x', 'y', 'z', 'w' };
	for (auto& token : desc.m_tokens)
	{
		Destination& dest = token.m_dest;
		destRegStr = regToString(dest.m_regType, dest.m_regNum, hasIndirect, tag);
		originalDestStr = destRegStr;

		OpLUT& lutentry = Mapping::agal2glsllut[token.m_opcode];
		unsigned char matrixHeight = lutentry.m_matrixHeight;
		for (unsigned char i = 0; i < matrixHeight; i++)
		{
			std::string line = lutentry.m_format;
			if (lutentry.m_dest)
			{
				std::string destMaskStr, destCastStr("float");
				if (matrixHeight > 1)
				{
					if (((dest.m_mask >> i) & 1) != 1)
						continue;

					destMaskStr = swizzles[i];
					destRegStr = "intt0.";
					destRegStr += destMaskStr;
				}
				else
				{
					if (dest.m_mask != 0xf)
					{
						unsigned char ndest = 0;
						if (dest.m_mask & 1)
						{
							ndest++;
							destMaskStr += 'x';
						}
						if (dest.m_mask & 2)
						{
							ndest++;
							destMaskStr += 'y';
						}
						if (dest.m_mask & 4)
						{
							ndest++;
							destMaskStr += 'z';
						}
						if (dest.m_mask & 8)
						{
							ndest++;
							destMaskStr += 'w';
						}
						destRegStr += '.';
						destRegStr += destMaskStr;

						switch (ndest)
						{
						case 1:
							destCastStr = "float";
							break;
						case 2:
							destCastStr = "vec2";
							break;
						case 3:
							destCastStr = "vec3";
							break;
						}
					}
					else
					{
						destCastStr = "vec4";
						destMaskStr = "xyzw";
					}
				}

				line.replace(0, 5, destRegStr);
				line.replace(line.find("%cast"), 5, destCastStr);
				std::string::size_type index = line.find("%dm");
				if (index != std::string::npos)
					line.replace(index, 3, destMaskStr);
			}
			
			unsigned char dwm = 0xf;
			if (!lutentry.m_ndwm && lutentry.m_dest)
				dwm = dest.m_mask;

			if (lutentry.m_a)
				line.replace(line.find("%a"), 2, sourceToString(token.m_src1, 0, dwm, lutentry.m_scalar, hasIndirect, tag));

			if (lutentry.m_b)
			{
				line.replace(line.find("%b"), 2, sourceToString(token.m_src2, i, dwm, lutentry.m_scalar, hasIndirect, tag));
				if (token.m_src2.m_regType == 5)
				{
					char texSize[2][5] = { "vec2", "vec3" };
					line.replace(line.find("%texdim"), 7, sampleTypes[token.m_src2.m_dim]);
					line.replace(line.find("%texsize"), 8, texSize[token.m_src2.m_dim]);
					line.replace(line.find("%lod"), 4, "");

#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
					if (token.m_src2.m_swizzle == 2)
					{
						// sample from alpha texture for ETC1 used on Android
						originalDestStr = destRegStr;
						line.replace(0, line.find("="), "intt0.xyz");
						line.replace(line.find("vec4"), 4, "vec3");
						line.replace(line.find(".xyzw"), 5, ".xyz");

						std::string str(line);
						str.replace(str.find(".xyz"), 4, ".w");
						str.replace(str.find("vec3"), 4, "float");
						str.replace(str.find("s"), 1, "w");
						str.replace(str.find(".xyz"), 4, ".x");
						oss << str;
					}
#endif
				}
			}

			oss << line;
		}

		if (matrixHeight == 3)
			oss << originalDestStr << ".xyz=vec3(intt0.xyz);\n";
		else if (matrixHeight == 4)
			oss << originalDestStr << "=vec4(intt0);\n";

#if AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
		if (token.m_opcode == 0x28 && token.m_src2.m_swizzle == 2)
			oss << originalDestStr << "=vec4(intt0);\n";
#endif
	}

	// adjust z from OpenGL range of [-1,1] to [0,1] as in D3D, this also enforces a left handed coordinate system
	if (tag == 'v')
		oss << "gl_Position=vec4(outpos.x,yflip*outpos.y,outpos.z*2.0-outpos.w,outpos.w);\n";

	oss << "}\n";

	return oss.str();
}

bool AGLSLParser::parseAGALByteArray(Description& desc, ByteArray& bytes)
{
	bytes.setPosition(0);
	if (bytes.readUnsignedByte() != 0xa0)
		return false; // Bad AGAL: Missing 0xa0 magic byte

	unsigned int version = bytes.readUnsignedInt();
	if (version >= 0x10)
	{
		bytes.readUnsignedByte();
		version >>= 1;
	}

	if (bytes.readUnsignedByte() != 0xa1)
		return false; // Bad AGAL: Missing 0xa1 magic byte

	desc.m_version = version;
	desc.m_shaderType = bytes.readUnsignedByte();

	unsigned int length = bytes.getLength();
	while (bytes.getPosition() < length)
	{
		Token token;
		token.m_opcode = bytes.readUnsignedInt();
		OpLUT& lutentry = Mapping::agal2glsllut[token.m_opcode];
		if (lutentry.m_matrixHeight)
			desc.m_hasMatrix = true;

		// dest
		if (lutentry.m_dest)
		{
			Destination& dest = token.m_dest;
			dest.m_regNum = bytes.readUnsignedShort();
			dest.m_mask = bytes.readUnsignedByte();
			dest.m_regType = bytes.readUnsignedByte();

			std::map<unsigned short, unsigned char>& regWrite = desc.m_regWrite[dest.m_regType];
			if (regWrite.find(dest.m_regNum) == regWrite.end())
				regWrite[dest.m_regNum] = dest.m_mask;
			else
				regWrite[dest.m_regNum] |= dest.m_mask;
		}
		else
			bytes.setPosition(bytes.getPosition() + 4);

		// src1
		if (lutentry.m_a)
			readReg(token.m_src1, 1, desc, bytes);
		else
			bytes.setPosition(bytes.getPosition() + 8);

		// src2
		if (lutentry.m_b)
			readReg(token.m_src2, lutentry.m_matrixHeight, desc, bytes);
		else
			bytes.setPosition(bytes.getPosition() + 8);

		desc.m_tokens.push_back(token);
	}

	return true;
}

void AGLSLParser::readReg(Destination& s, unsigned char mh, Description& desc, ByteArray& bytes)
{
	s.m_regNum = bytes.readUnsignedShort();
	s.m_indexOffset = bytes.readByte(); // this is LOD bias if m_regType=5
	s.m_swizzle = bytes.readUnsignedByte(); // this is not used if m_regType=5
	s.m_regType = bytes.readUnsignedByte();

	if (s.m_regType == 5)
	{
		// sampler
		//s.m_lodBias = s.m_indexOffset; // Texture level-of-detail (LOD) bias, signed integer, scale by 8. The floating point value used is b/8.0 (8 bits)
		unsigned char byte = bytes.readUnsignedByte();
		s.m_swizzle = byte & 0xf; // used as TextureFormat if m_regType=5
		s.m_dim = byte >> 4;
		bytes.readUnsignedByte(); // Wrapping (0=clamp,1=repeat) + Special flag bits (must be 0)
		bytes.readUnsignedByte(); // Filter (0=nearest,1=linear) + Mipmap (0=disable,1=nearest, 2=linear)
		desc.m_samplers[s.m_regNum] = byte;
		s.m_indirectFlag = 0;
	}
	else
	{
		s.m_indexRegType = bytes.readUnsignedByte();
		s.m_indexSelect = bytes.readUnsignedByte();
		s.m_indirectFlag = bytes.readUnsignedByte() >> 7;

		if (s.m_indirectFlag)
		{
			desc.m_hasIndirect = true;
			desc.m_regRead[s.m_indexRegType][s.m_regNum] = 0xf;
		}
		else
		{
			desc.m_regRead[s.m_regType][s.m_regNum] = 0xf; // sould be swizzle to mask? should be |=
			for (unsigned char i = 1; i < mh; i++)
				desc.m_regRead[s.m_regType][s.m_regNum + i] = desc.m_regRead[s.m_regType][s.m_regNum];
		}
	}
}

std::string AGLSLParser::regToString(unsigned int regType, unsigned int regNum, bool hasIndirect, char tag)
{
	std::ostringstream oss;
	switch (regType)
	{
	case 0:
		oss << "va" << regNum;
		break;
	case 1:
		if (hasIndirect && tag == 'v')
			oss << "vcarrr[" << regNum << "]";
		else
			oss << tag << "c" << regNum;
		break;
	case 2:
		oss << tag << "t" << regNum;
		break;
	case 3:
		oss << (tag == 'v' ? "outpos" : "gl_FragColor");
		break;
	case 4:
		oss << "v" << regNum;
		break;
	case 5:
		oss << "fs" << regNum;
		break;
	}

	return oss.str();
}

std::string AGLSLParser::sourceToString(Destination& s, int subline, unsigned int dwm, bool isScalar, bool hasIndirect, char tag)
{
	std::ostringstream oss;
	char swizzles[4] = { 'x', 'y', 'z', 'w' };
	if (s.m_indirectFlag)
	{
		oss << "vcarrr[int("
			<< regToString(s.m_indexRegType, s.m_regNum, hasIndirect, tag)
			<< "."
			<< swizzles[s.m_indexSelect]
			<< ")";

		int realofs = subline + s.m_indexOffset;
		if (realofs < 0)
			oss << realofs;
		
		if (realofs > 0)
			oss << "+" << realofs;
		
		oss << "]";
	}
	else
	{
		oss << regToString(s.m_regType, s.m_regNum + subline, hasIndirect, tag);
	}

	// samplers never add swizzle
	if (s.m_regType == 5)
		return oss.str();

	// scalar, first component only
	if (isScalar)
	{
		oss << "." << swizzles[s.m_swizzle & 3];
		return oss.str();
	}

	// identity
	if (s.m_swizzle == 0xe4 && dwm == 0xf)
		return oss.str();

	// with destination write mask folded in
	oss << ".";
	if (dwm & 1)
		oss << swizzles[s.m_swizzle & 3];
	
	if (dwm & 2)
		oss << swizzles[(s.m_swizzle >> 2) & 3];
	
	if (dwm & 4)
		oss << swizzles[(s.m_swizzle >> 4) & 3];
	
	if (dwm & 8)
		oss << swizzles[(s.m_swizzle >> 6) & 3];

	return oss.str();
}