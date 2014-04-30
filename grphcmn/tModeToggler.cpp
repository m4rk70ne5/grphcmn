#include "tModeToggler.h"

tModeToggler* tModeToggler::m_instance = NULL;

tModeToggler* tModeToggler::GetModeToggler()
{
	if (m_instance == NULL)
	{
		m_instance = new tModeToggler();
	}
	return m_instance;
}

tModeToggler::~tModeToggler()
{
	m_instance = NULL;
}

void tModeToggler::SetCurrentDemo(std::string dem)
{
	if (!dem.compare("OctreeDemo"))
	{
		// octree demo has a wireframe mode
		m_bbDraw = false;
		m_OctDraw = false;
		m_lastPressed[SDL_SCANCODE_W] = std::pair<bool, bool*>(false, &m_bbDraw);
		m_lastPressed[SDL_SCANCODE_O] = std::pair<bool, bool*>(false, &m_OctDraw);
	}
}

bool tModeToggler::Check(int scancode, const Uint8* keys)
{
	bool pressed = keys[scancode];
	if (pressed)
	{
		bool previouslyPressed = m_lastPressed[scancode].first;
		if (!previouslyPressed)
		{
			m_lastPressed[scancode].first = true;
			bool* toggle = m_lastPressed[scancode].second;
			*toggle = !*toggle;
		}
	}
	else
	{
		m_lastPressed[scancode].first = false;
	}
	return pressed;
}

void tModeToggler::Update()
{
	const Uint8 *keyState = SDL_GetKeyboardState(NULL);
	for (auto i = m_lastPressed.begin(); i != m_lastPressed.end(); i++)
	{
		Check(i->first, keyState);
	}
}