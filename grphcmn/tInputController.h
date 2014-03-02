#ifndef TINPUTCONTROLLER_H
#define TINPUTCONTROLLER_H

#include <map>
#include <vector>

class tObject;

class tInputController
{
protected:
	std::map<int, std::vector<tObject*>> m_attachedKObjects;
	std::vector<tObject*> m_mouseReactors;
	static tInputController* m_instance;
	int m_mouseBoundX, m_mouseBoundY, m_cursorPosX, m_cursorPosY;
public:
	static tInputController* GetInputController();
	virtual ~tInputController() { m_instance = NULL; };
	std::map<int, bool> m_pushedKeys;
	void AttachKObject(tObject*, std::vector<int>&); // the object will handle key
	void AttachMObject(tObject* obj) { m_mouseReactors.push_back(obj); };
	void SetMouseBounds(int width, int height) { m_mouseBoundX = width; m_mouseBoundY = height; };
	void HandleInput();
};

#endif