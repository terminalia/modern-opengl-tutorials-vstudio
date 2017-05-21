#include "shaderprogram.h"

ShaderProgram::ShaderProgram()
{

}

ShaderProgram::ShaderProgram(const std::string &name, const std::string &vertexShaderFile, const std::string &fragmentShaderFile)
{
	this->name = name;
	this->vertexShaderFile = vertexShaderFile;
	this->fragmentShaderFile = fragmentShaderFile;
	init();
}

ShaderProgram::ShaderProgram(const ShaderProgram &shaderProgram)
{
	name = shaderProgram.name;
	vertexShaderFile = shaderProgram.vertexShaderFile;
	fragmentShaderFile = shaderProgram.fragmentShaderFile;
	programId = shaderProgram.programId;
}

ShaderProgram::~ShaderProgram()
{

}

void ShaderProgram::init()
{
	std::string vertexSource = loadSourceFromFile(vertexShaderFile);
	std::string fragmentSource = loadSourceFromFile(fragmentShaderFile);

	if (vertexSource.empty()) {
		throw std::runtime_error("Shader '" + name + "' failed loading vertex source file '" + vertexShaderFile + "'!");
	}

	if (fragmentSource.empty()) {
		throw std::runtime_error("Shader '" + name + "' failed loading fragment source file '" + fragmentShaderFile + "'!");
	}

	GLuint vertexShader = addShader(vertexSource, GL_VERTEX_SHADER);
	GLuint fragmentShader = addShader(fragmentSource, GL_FRAGMENT_SHADER);
	link(vertexShader, fragmentShader);

	glDetachShader(programId, vertexShader);
	glDetachShader(programId, fragmentShader);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

}

void ShaderProgram::update()
{
	cleanUp();
	init();
}

GLuint ShaderProgram::addShader(const std::string &shaderSource, GLenum type)
{
	const char *source = shaderSource.c_str();

	GLuint shaderId = glCreateShader(type);
	glShaderSource(shaderId, 1, &source, NULL);
	checkCompilationErrors(shaderId, type);

	return shaderId;
}

void ShaderProgram::link(GLuint vertexShader, GLuint fragmentShader)
{
	programId = glCreateProgram();
	glAttachShader(programId, vertexShader);
	glAttachShader(programId, fragmentShader);
	glLinkProgram(programId);
	checkLinkErros();
}

std::string ShaderProgram::loadSourceFromFile(const std::string &source_file)
{
	std::ifstream fileStream;
	std::string source = "";

	fileStream.open(source_file.c_str(), std::ios_base::in);
	if (fileStream)
	{
		source = std::string(std::istreambuf_iterator<char>(fileStream), (std::istreambuf_iterator<char>()));
		fileStream.close();
		return source;
	}

	return "";
}

void ShaderProgram::checkCompilationErrors(GLuint shaderId, GLenum type)
{
	GLint isCompiled = 0;
	glCompileShader(shaderId);
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint infoLogLength;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetShaderInfoLog(shaderId, infoLogLength, NULL, infoLog);
		std::string compilationErrors;


		if (type == GL_VERTEX_SHADER)
		{
			compilationErrors = "'" + name + "' vertex compilation errors: ";

		}
		else if (type == GL_FRAGMENT_SHADER)
		{
			compilationErrors = "'" + name + "' fragment compilation errors: ";
		}

		compilationErrors.append("\n");
		compilationErrors.append(std::string(infoLog));

		delete[] infoLog;

		throw std::runtime_error(compilationErrors);
	}
}

void ShaderProgram::checkLinkErros()
{
	GLint isLinked = 0;
	glGetProgramiv(programId, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint infoLogLength;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar *infoLog = new GLchar[infoLogLength];
		glGetProgramInfoLog(programId, infoLogLength, NULL, infoLog);
		std::cerr << infoLog << std::endl;
		std::string linkErrors;


		linkErrors = "ShaderProgram " + name + " linking failed!\n";
		linkErrors.append(std::string(infoLog));
		delete[] infoLog;

		throw std::runtime_error(linkErrors);
	}
}

void ShaderProgram::cleanUp()
{
	glDeleteProgram(programId);
}

GLuint ShaderProgram::getProgram() const
{
	return programId;
}

std::string ShaderProgram::getName()
{
	return name;
}
