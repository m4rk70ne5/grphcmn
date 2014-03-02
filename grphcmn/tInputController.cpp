#include "tInputController.h"
#include "tObject.h"
#include <SDL2/SDL.h>

tInputController* tInputController::m_instance = NULL;

tInputController* tInputController::GetInputController()
{
	if (m_instance == NULL)
	{
		m_instance = new tInputController();
	}
	return m_instance;
}

void tInputController::AttachKObject(tObject* pObj, std::vector<int>& keycode)
{
	for (std::vector<int>::iterator iKey = keycode.begin(); iKey != keycode.end(); ++iKey)
	{
		std::map<int, std::vector<tObject*>>::iterator result = m_attachedKObjects.find(*iKey);
		if (result == m_attachedKObjects.end())
		{
			m_attachedKObjects[*iKey] = std::vector<tObject*>();
		}
		m_attachedKObjects[*iKey].push_back(pObj);
	}
}

void tInputController::HandleInput()
{
	// Get the mouse's current X,Y position
    SDL_GetMouseState(&m_cursorPosX, &m_cursorPosY);

	// handle mouse (the handler will have arguments x,y for cursor position)
	for (auto iMouseReactors = m_mouseReactors.begin(); iMouseReactors != m_mouseReactors.end(); ++iMouseReactors)
	{
		(*iMouseReactors)->MouseHandler(m_cursorPosX, m_cursorPosY, m_mouseBoundX, m_mouseBoundY);
	}
	// handle keyboard
	/*for (auto iKeys = m_pushedKeys.begin(); iKeys != m_pushedKeys.end(); ++iKeys)
	{
		std::vector<tObject*>& kvec = m_attachedKObjects[iKeys->first];
		for (auto iObj = kvec.begin(); iObj != kvec.end(); ++iObj)
		{
			(*iObj)->Handler(iKeys->first, iKeys->second);
		}
	}*/
}