#include "Program.h"
#include "AGLSLParser.h"

#if AWAY_PLATFORM == AWAY_PLATFORM_WINDOWS
# if defined(AWAY_PLUGIN_ANGLE)
#  include "GLES2/gl2.h"
# else
#  include "glew.h"
# endif
#elif AWAY_PLATFORM == AWAY_PLATFORM_OSX
# include <OpenGL/gl.h>
# include <GLUT/glut.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_LINUX
# include <GL/gl.h>
# include <GL/glu.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_IOS
# include <OpenGLES/ES2/gl.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_ANDROID
# include <GLES2/gl2.h>
#elif AWAY_PLATFORM == AWAY_PLATFORM_HTML5
# include <GLES2/gl2.h>
# include <EGL/egl.h>
#endif

USING_AWAY_NAMESPACE

Program::Program()
{
	m_program = glCreateProgram();
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glAttachShader(m_program, m_vertexShader);
	glAttachShader(m_program, m_fragmentShader);
}

Program::~Program()
{
	glDeleteProgram(m_program);
}

void Program::upload(ByteArray& vertexProgram, ByteArray& fragmentProgram)
{
	// generate and compile vertex shader
	std::string vertexString = AGLSLParser::parse(vertexProgram, m_hasVertexIndirect);
#ifdef GL_ES_VERSION_2_0
	std::string src = "#version 100\n" + vertexString;
#else
	std::string src = "#version 120\n" + vertexString;
#endif
	const char* sourceString = src.c_str();
	glShaderSource(m_vertexShader, 1, &sourceString, nullptr);
	glCompileShader(m_vertexShader);

#ifdef _DEBUG
	std::cout << "Vertex Shader:" << std::endl;
	std::cout << sourceString << std::endl;
	GLint compiled;
	glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(m_vertexShader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 0)
		{
			std::vector<char> logs(infoLen);
			glGetShaderInfoLog(m_vertexShader, infoLen, nullptr, logs.data());
			std::cout << "Error compiling shader:" << logs.data() << std::endl;
		}
	}
#endif

	// generate and compile fragment shader
	std::string fragmentString = AGLSLParser::parse(fragmentProgram, m_hasFragmentIndirect);
#ifdef GL_ES_VERSION_2_0
	src = "#version 100\n" + fragmentString;
#else
	src = "#version 120\n" + fragmentString;
#endif
	sourceString = src.c_str();
	glShaderSource(m_fragmentShader, 1, &sourceString, nullptr);
	glCompileShader(m_fragmentShader);

#ifdef _DEBUG
	std::cout << "Fragment Shader:" << std::endl;
	std::cout << sourceString << std::endl;
	glGetShaderiv(m_fragmentShader, GL_COMPILE_STATUS, &compiled);
	if (!compiled)
	{
		GLint infoLen = 0;
		glGetShaderiv(m_fragmentShader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 0)
		{
			std::vector<char> logs(infoLen);
			glGetShaderInfoLog(m_fragmentShader, infoLen, nullptr, logs.data());
			std::cout << "Error compiling shader:" << logs.data() << std::endl;
		}
	}
#endif

	// link vertex shader and fragment shader
	glLinkProgram(m_program);

#ifdef _DEBUG
	GLint linked;
	glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 0)
		{
			std::vector<char> logs(infoLen);
			glGetProgramInfoLog(m_program, infoLen, nullptr, logs.data());
			std::cout << "Error linking program:" << logs.data() << std::endl;
		}
	}
#endif

	// get all active uniforms
	GLenum type;
	int numUniforms, maxUniformLen, len, size;
	glGetProgramiv(m_program, GL_ACTIVE_UNIFORMS, &numUniforms);
	glGetProgramiv(m_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLen);
	for (int i = 0; i < numUniforms; i++)
	{
		std::vector<char> name(maxUniformLen);
		glGetActiveUniform(m_program, i, maxUniformLen, &len, &size, &type, name.data());
		m_uniforms[std::string(name.data(), len)] = glGetUniformLocation(m_program, name.data());
	}

	// get all active attributes
	int numAttributes, maxAttributeLen;
	glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTES, &numAttributes);
	glGetProgramiv(m_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttributeLen);
	for (int i = 0; i < numAttributes; i++)
	{
		std::vector<char> name(maxAttributeLen);
		glGetActiveAttrib(m_program, i, maxAttributeLen, &len, &size, &type, name.data());
		m_attributes[std::string(name.data(), len)] = glGetAttribLocation(m_program, name.data());
	}
}

int Program::getUniformLocation(const std::string& name)
{
	if (m_uniforms.count(name) == 0)
		return -1;

	return m_uniforms[name];
}

int Program::getAttributeLocation(const std::string& name)
{
	if (m_attributes.count(name) == 0)
		return -1;

	return m_attributes[name];
}