#ifndef TGRAPHICS_CONTROLLERH
#define TGRAPHICS_CONTROLLERH

#include <SDL2/SDL.h>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include "tMatrix.h"

class tGeometry;

// singleton
class tGraphicsController
{
protected:
	SDL_Window* m_window; // every demo has a window
	SDL_GLContext m_context;
	static tGraphicsController* m_instance;
	glm::mat4* m_camMat;
	glm::mat4 m_projMat;
public:
	virtual ~tGraphicsController();
	static tGraphicsController* GetGraphicsController();
	void Initialize(const char*, int, int);
	void Draw(std::map<int, std::vector<tGeometry*>>&, std::map<int, std::string>&); // pass geometry into controller
	void SetProjectionMatrix(glm::mat4 mat) { m_projMat = mat; };
	glm::mat4 GetProjectionMatrix() { return m_projMat; };
	void SetCamMatrix(glm::mat4* camMat) { m_camMat = camMat; };
};

#endif