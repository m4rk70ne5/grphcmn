#ifndef TGAMECLOCK_H
#define TGAMECLOCK_H

#include <vector>
#include <map>
#include <string>

class tGraphicsController;
class tInputController;
class tObject;
class tGeometry;

struct tQuitStructure
{
	// handles quits
	unsigned int (__stdcall* m_StopFunction) (void*);
	void* m_argument;
};

class tGameClock
{
protected:
	float m_curFrameEnd, m_frameLength, m_minFrames;
	bool m_done;
public:
	tGameClock(int, int);
	void Update();
	void Loop();
	void Stop() { m_done = true; };
	tGraphicsController* m_GC;
	tInputController* m_IC;
	tQuitStructure m_quitStruct;
	std::vector<tObject*>* m_dynamicObjects;
	std::map<int, std::vector<tGeometry*>>* m_geoContainer;
	std::map<int, std::string>* m_vaoProgram;
};

#endif