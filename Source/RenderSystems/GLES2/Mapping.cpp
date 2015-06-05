#include "Mapping.h"

USING_AWAY_NAMESPACE

OpLUT Mapping::agal2glsllut[46] = {
		// format,                                                   dest,     a,      b, mh, ndwm, scalar
		{ "%dest=%cast(%a);\n",                                      true,  true,  false, 1, false, false }, // mov
		{ "%dest=%cast(%a+%b);\n",                                   true,  true,  true,  1, false, false }, // add
		{ "%dest=%cast(%a-%b);\n",                                   true,  true,  true,  1, false, false }, // sub
		{ "%dest=%cast(%a*%b);\n",                                   true,  true,  true,  1, false, false }, // mul
		{ "%dest=%cast(%a/%b);\n",                                   true,  true,  true,  1, false, false }, // div
		{ "%dest=%cast(1.0)/%a;\n",                                  true,  true,  false, 1, false, false }, // rcp
		{ "%dest=%cast(min(%a,%b));\n",                              true,  true,  true,  1, false, false }, // min
		{ "%dest=%cast(max(%a,%b));\n",                              true,  true,  true,  1, false, false }, // max
		{ "%dest=%cast(fract(%a));\n",                               true,  true,  false, 1, false, false }, // frc
		{ "%dest=%cast(sqrt(abs(%a)));\n",                           true,  true,  false, 1, false, false }, // sqt
		{ "%dest=%cast(inversesqrt(abs(%a)));\n",                    true,  true,  false, 1, false, false }, // rsq
		{ "%dest=%cast(pow(abs(%a),%b));\n",                         true,  true,  true,  1, false, false }, // pow
		{ "%dest=%cast(log2(abs(%a)));\n",                           true,  true,  false, 1, false, false }, // log
		{ "%dest=%cast(exp2(%a));\n",                                true,  true,  false, 1, false, false }, // exp
		{ "%dest=%cast(normalize(vec3(%a)));\n",                     true,  true,  false, 1, true,  false }, // nrm
		{ "%dest=%cast(sin(%a));\n",                                 true,  true,  false, 1, false, false }, // sin
		{ "%dest=%cast(cos(%a));\n",                                 true,  true,  false, 1, false, false }, // cos
		{ "%dest=%cast(cross(vec3(%a),vec3(%b)));\n",                true,  true,  true,  1, true,  false }, // crs
		{ "%dest=%cast(dot(vec3(%a),vec3(%b)));\n",                  true,  true,  true,  1, true,  false }, // dp3
		{ "%dest=%cast(dot(vec4(%a),vec4(%b)));\n",                  true,  true,  true,  1, true,  false }, // dp4
		{ "%dest=%cast(abs(%a));\n",                                 true,  true,  false, 1, false, false }, // abs
		{ "%dest=%cast(%a*-1.0);\n",                                 true,  true,  false, 1, false, false }, // neg
		{ "%dest=%cast(clamp(%a,0.0,1.0));\n",                       true,  true,  false, 1, false, false }, // sat
		{ "%dest=%cast(dot(vec3(%a),vec3(%b)));\n",                  true,  true,  true,  3, true,  false }, // m33
		{ "%dest=%cast(dot(vec4(%a),vec4(%b)));\n",                  true,  true,  true,  4, true,  false }, // m44
		{ "%dest=%cast(dot(vec4(%a),vec4(%b)));\n",                  true,  true,  true,  3, true,  false }, // m34
		{ "%dest=%cast(dFdx(%a));\n",                                true,  true,  false, 1, false, false }, // ddx
		{ "%dest=%cast(dFdy(%a));\n",                                true,  true,  false, 1, false, false }, // ddy
		{ "if(float(%a)==float(%b)){;\n",                            false, true,  true,  1, false, true },  // ife
		{ "if(float(%a)!=float(%b)){;\n",                            false, true,  true,  1, false, true },  // ine
		{ "if(float(%a)>=float(%b)){;\n",                            false, true,  true,  1, false, true },  // ifg
		{ "if(float(%a)<float(%b)){;\n",                             false, true,  true,  1, false, true },  // ifl
		{ "}else{;\n",                                               false, false, false, 1, false, false }, // els
		{ "};\n",                                                    false, false, false, 1, false, false }, // eif
		{ "",                                                        false, false, false, 1, false, false }, // 0x22 not used
		{ "",                                                        false, false, false, 1, false, false }, // 0x23 not used
		{ "",                                                        false, false, false, 1, false, false }, // 0x24 not used
		{ "",                                                        false, false, false, 1, false, false }, // 0x25 not used
		{ "%dest=%cast(texture%texdimLod(%b,%texsize(%a)).%dm);\n",  true,  true,  true,  1, false, false }, // ted
		{ "if(float(%a)<0.0) discard;\n",                            false, true,  false, 1, false, true },  // kil
		{ "%dest=%cast(texture%texdim(%b,%texsize(%a)%lod).%dm);\n", true,  true,  true,  1, true,  false }, // tex
		{ "%dest=%cast(greaterThanEqual(%a,%b).%dm);\n",             true,  true,  true,  1, true,  false }, // sge
		{ "%dest=%cast(lessThan(%a,%b).%dm);\n",                     true,  true,  true,  1, true,  false }, // slt
		{ "%dest=%cast(sign(%a));\n",                                true,  true,  false, 1, false, false }, // sgn
		{ "%dest = %cast(equal(%a,%b).%dm);\n",                      true,  true,  true,  1, true,  false }, // seq
		{ "%dest = %cast(notEqual(%a,%b).%dm);\n",                   true,  true,  true,  1, true,  false }, // sne
};