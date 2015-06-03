#include "Tex.h"

USING_AWAY_NAMESPACE

const unsigned int Tex::D2 = 0;
const unsigned int Tex::CUBE = 0x10000;
const unsigned int Tex::D3 = 0x20000;

const unsigned int Tex::NOMIP = 0;
const unsigned int Tex::MIPNONE = 0;
const unsigned int Tex::MIPNEAREST = 0x4000000;
const unsigned int Tex::MIPLINEAR = 0x8000000;

const unsigned int Tex::NEAREST = 0;
const unsigned int Tex::LINEAR = 0x20000000;
const unsigned int Tex::ANISOTROPIC2X = 0x40000000;
const unsigned int Tex::ANISOTROPIC4X = 0x60000000;
const unsigned int Tex::ANISOTROPIC8X = 0x80000000;
const unsigned int Tex::ANISOTROPIC16X = 0xA0000000;

const unsigned int Tex::CENTROID = 0x80000;
const unsigned int Tex::SINGLE = 0x100000;
const unsigned int Tex::IGNORESAMPLER = 0x200000;

const unsigned int Tex::REPEAT = 0x800000;
const unsigned int Tex::WRAP = 0x800000;
const unsigned int Tex::CLAMP = 0;
const unsigned int Tex::CLAMP_U_REPEAT_V = 0x1000000;
const unsigned int Tex::REPEAT_U_CLAMP_V = 0x1800000;

const unsigned int Tex::RGBA = 0;
const unsigned int Tex::DXT1 = 0x2000;
const unsigned int Tex::DXT5 = 0x4000;
const unsigned int Tex::VIDEO = 0x6000;