#ifndef TMODETOGGLER_H
#define TMODETOGGER_H

#include "tObject.h"
#include <string>
#include <map>
#include <SDL2/SDL.h>

class tModeToggler : public tObject
{
protected:
	std::map<int, std::pair<bool, bool*>> m_lastPressed; // used to eliminate problems when key is held down too long
	bool Check(int, const Uint8*); // check for scancode, set boolean to appropriate value
	static tModeToggler* m_instance; // singleton
public:
	// list of toggle booleans
	bool m_bbDraw;
	bool m_OctDraw;
	// functions
	static tModeToggler* GetModeToggler();
	~tModeToggler();
	void SetCurrentDemo(std::string);
	virtual void Update(); // keypresses
};

#endif