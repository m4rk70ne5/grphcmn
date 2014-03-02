#include "tGraphicsController.h"
#include "tProgramManager.h"
#include "tGeometry.h"
#include <gl/glew.h>
#include <gl/wglew.h>
#include <glm/gtc/type_ptr.hpp>

tGraphicsController* tGraphicsController::m_instance = NULL;

tGraphicsController* tGraphicsController::GetGraphicsController()
{
	if (m_instance == NULL)
	{
		m_instance = new tGraphicsController();
	}
	return m_instance;
}

tGraphicsController::~tGraphicsController()
{
	// close out the subsystem
	SDL_GL_DeleteContext(m_context);
	// Close and destroy the window
    SDL_DestroyWindow(m_window);

	m_instance = NULL;
}
void tGraphicsController::Draw(std::map<int, std::vector<tGeometry*>>& geometry, std::map<int, std::string>& vaoProg)
{
	// cycle through vector of drawable objects (tGeometry)
	for (auto i = geometry.begin(); i != geometry.end(); i++)
	{
		// bind vertex array
		glBindVertexArray(i->first);
		
		// use program
		tProgramManager* pProgramManager = tProgramManager::GetProgramManager();
		int prog = pProgramManager->GetProgram(vaoProg[i->first]);
		glUseProgram(prog);
		
		// set the camera matrix
		GLint camLoc = glGetUniformLocation(prog, "camera");
		glProgramUniformMatrix4fv(prog, camLoc, 1, GL_FALSE, glm::value_ptr(*m_camMat));
		
		// cycle through meshes
		for (auto j = i->second.begin(); j != i->second.end(); j++)
		{
			// set uniforms, call overloaded function
			(*j)->SetUniforms(prog);
			// draw elements
			(*j)->Draw(prog);
		}
	}
	SDL_GL_SwapWindow(m_window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void tGraphicsController::Initialize(const char* title, int width, int height)
{
	// initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

	// try for a 4.3 context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// create the window
    m_window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          width, height, SDL_WINDOW_OPENGL);
	
	// Create an OpenGL context associated with the window.
    m_context = SDL_GL_CreateContext(m_window);

	//set up glew, now that a context has been created with sdl createcontext
	glewExperimental = GL_TRUE;
	GLenum err = glewInit(); //now intialize glew

	// Initialize GL how you'd like it
	glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
	
	SDL_GL_SwapWindow(m_window);
	glClear(GL_COLOR_BUFFER_BIT); // call clear again so we can get rid of that flickering -- sets back buffer to clear color

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

	// glviewport
	glViewport(0, 0, width, height);

	// hide the cursor
	SDL_ShowCursor(false);
}