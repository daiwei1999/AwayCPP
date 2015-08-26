#include "RadialBlurFilterTask.h"
#include "IContext.h"
#include "Shader.h"
#include "Regs.h"

USING_AWAY_NAMESPACE

#define LAYERS 15

RadialBlurFilterTask::RadialBlurFilterTask(float intensity, float glowGamma, float blurStart, float blurWidth, float cx, float cy)
{
	m_data[0] = intensity;
	m_data[1] = glowGamma;
	m_data[2] = blurStart;
	m_data[3] = blurWidth;
	m_data[4] = cx;
	m_data[5] = cy;
	m_data[6] = m_data[7] = m_data[8] = 0;
	m_data[9] = 1;
	m_data[10] = LAYERS;
	m_data[11] = LAYERS - 1;
}

void RadialBlurFilterTask::activate(IContext* context, Camera3D* camera, Texture* depthTexture)
{
	context->setProgramConstantsFromVector(ProgramType::VERTEX, 0, m_data, 3);
	context->setProgramConstantsFromVector(ProgramType::FRAGMENT, 0, m_data, 3);
}

void RadialBlurFilterTask::getVertexCode(Shader& vertexShader)
{
	vertexShader.mov(Regs::op, Regs::a0);
	vertexShader.mov(Regs::t0, Regs::c2 ^ Regs::xxxy);
	vertexShader.sub(Regs::t0 ^ Regs::xy, Regs::a1 ^ Regs::xy, Regs::c1 ^ Regs::xy);
	vertexShader.mov(Regs::v0, Regs::t0);
}

void RadialBlurFilterTask::getFragmentCode(Shader& fragmentShader)
{
	fragmentShader.mov(Regs::t1, Regs::c2 ^ Regs::x);
	fragmentShader.mov(Regs::t2 ^ Regs::xy, Regs::c1 ^ Regs::xy);
	fragmentShader.mov(Regs::t3 ^ Regs::x, Regs::c2 ^ Regs::x);

	// Y-Axis
	for (int i = 0; i <= LAYERS; i++)
	{
		// float scale = BlurStart + BlurWidth*(i/(float) (nsamples-1)); -> ft4
		// ft4.x = (i/(float)(nsamples-1))
		fragmentShader.div(Regs::t4 ^ Regs::x, Regs::t3 ^ Regs::x, Regs::c2 ^ Regs::w);
		// ft4.x *= Blurwidth;
		fragmentShader.mul(Regs::t4 ^ Regs::x, Regs::t4 ^ Regs::x, Regs::c0 ^ Regs::w);
		// ft4.x += BlurStart;
		fragmentShader.add(Regs::t4 ^ Regs::x, Regs::t4 ^ Regs::x, Regs::c0 ^ Regs::z);
		// blurred += tex2D(tex, IN.UV.xy*scale + ctrPt );
		fragmentShader.mov(Regs::t5 ^ Regs::xy, Regs::v0 ^ Regs::xy);
		fragmentShader.mul(Regs::t5 ^ Regs::xy, Regs::t5 ^ Regs::xy, Regs::t4 ^ Regs::x);
		fragmentShader.add(Regs::t5 ^ Regs::xy, Regs::t5 ^ Regs::xy, Regs::c1 ^ Regs::xy);
		fragmentShader.tex(Regs::t5, Regs::t5 ^ Regs::xy, Regs::s0);
		fragmentShader.add(Regs::t1, Regs::t1, Regs::t5);
		// inc counter by one
		fragmentShader.add(Regs::t3 ^ Regs::x, Regs::t3 ^ Regs::x, Regs::c2 ^ Regs::y);
	}

	// blurred /= nsamples;
	fragmentShader.div(Regs::t1, Regs::t1, Regs::c2 ^ Regs::z);
	// blurred.rgb = pow(blurred.rgb,GlowGamma);
	fragmentShader.pow(Regs::t1 ^ Regs::xyz, Regs::t1 ^ Regs::xyz, Regs::c0 ^ Regs::y);
	// blurred.rgb *= Intensity;
	fragmentShader.mul(Regs::t1 ^ Regs::xyz, Regs::t1 ^ Regs::xyz, Regs::c0 ^ Regs::x);
	// blurred.rgb = saturate(blurred.rgb);
	fragmentShader.sat(Regs::t1 ^ Regs::xyz, Regs::t1 ^ Regs::xyz);
	// origTex = tex2D(tex, IN.UV.xy + ctrPt);
	fragmentShader.add(Regs::t0 ^ Regs::xy, Regs::v0 ^ Regs::xy, Regs::c1 ^ Regs::xy);
	fragmentShader.tex(Regs::t6, Regs::t0 ^ Regs::xy, Regs::s0);
	// newC = origTex.rgb + blurred.rgb;
	fragmentShader.add(Regs::t1 ^ Regs::xyz, Regs::t1 ^ Regs::xyz, Regs::t6 ^ Regs::xyz);
	// oc = newC
	fragmentShader.mov(Regs::oc, Regs::t1);
}