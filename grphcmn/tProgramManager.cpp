#include "tProgramManager.h"

tProgramManager* tProgramManager::m_instance = NULL;

tProgramManager::tProgramManager()
{
}

tProgramManager* tProgramManager::GetProgramManager()
{
	if (m_instance == NULL)
		m_instance = new tProgramManager();
	return m_instance;
}

void tProgramManager::AddProgram(string programName, int programID)
{
	m_programHash[programName] = programID;
}

int tProgramManager::CreateProgram(string vertexShader, string fragmentShader, string geometryShader)
{
	GLuint program = glCreateProgram(); //first create the program
	GLuint vertShader, fragShader, geoShader;
	GLchar* vertShaderString, *fragShaderString, *geoShaderString;

	//create shaders
	if (vertexShader != "")
	{
		//vertex shader
		ifstream vertFile;
		vertShader = glCreateShader(GL_VERTEX_SHADER);
		vertFile.open(vertexShader.c_str(), ios::binary);
		vertShaderString = FileToString(&vertFile);
		vertFile.close();
		//now that we have the vertex shader file's contents in a string
		//let's compile it and see what happens
		//it won't indicate if it didn't compile correctly
		//so you must first compile this shader using a separate shader compiler
		//that tells you your errors

		glShaderSource(vertShader, 1, (const GLchar**)&vertShaderString, NULL);
		glCompileShader(vertShader); //vertex shader

		//now attach it to the program
		glAttachShader(program, vertShader);
	}

	if (fragmentShader != "")
	{
		//fragment shader
		ifstream fragFile;
		fragShader = glCreateShader(GL_FRAGMENT_SHADER);
		fragFile.open(fragmentShader.c_str(), ios::binary);
		fragShaderString = FileToString(&fragFile);
		fragFile.close();

		glShaderSource(fragShader, 1, (const GLchar**)&fragShaderString, NULL);
		glCompileShader(fragShader); //fragment shader

		glAttachShader(program, fragShader);
	}
	if (geometryShader != "")
	{
		//fragment shader
		ifstream geoFile;
		geoShader = glCreateShader(GL_GEOMETRY_SHADER);
		geoFile.open(geometryShader.c_str(), ios::binary);
		geoShaderString = FileToString(&geoFile);
		geoFile.close();

		glShaderSource(geoShader, 1, (const GLchar**)&geoShaderString, NULL);
		glCompileShader(geoShader); //fragment shader

		glAttachShader(program, geoShader);
	}	

	glLinkProgram(program); //now try linking the program

	//error checking
	GLint status;
    glGetProgramiv (program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint infoLogLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);
        
        GLchar *strInfoLog = new GLchar[infoLogLength + 1];
        glGetProgramInfoLog(program, infoLogLength, NULL, strInfoLog);
        fprintf(stderr, "Linker failure: %s\n", strInfoLog);
        delete[] strInfoLog;
		program = -1; //this will indicate an error
    }
	if (vertexShader != "")
	{
		glDetachShader(program, vertShader);
		free((void*)vertShaderString); //free heap-allocated string memory blocks
	}
	if (fragmentShader != "")
	{
		glDetachShader(program, fragShader);
		free((void*)fragShaderString);
	}
	if (geometryShader != "")
	{
		glDetachShader(program, geoShader);
		free((void*)geoShaderString);
	}
	return program; //this is the id of the program
}

GLchar* tProgramManager::FileToString(ifstream* pIf)
{
	if (pIf->good())
	{
		//get file size
		pIf->seekg(0, ios::end);
		streampos length = pIf->tellg();
		pIf->seekg(0, ios::beg);

		//allocate enough space
		GLchar* shaderString = (char*)malloc((int)length + 1);

		//read in the file
		pIf->read((GLchar*)shaderString, length);
		shaderString[(int)length] = '\0'; //null-terminate it

		return shaderString;
	}
	else
		return NULL;
}

int tProgramManager::GetProgram(string name)
{
	return m_programHash[name];
}