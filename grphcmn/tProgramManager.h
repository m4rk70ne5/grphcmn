#ifndef TPROGRAMMANAGER_H
#define TPROGRAMMANAGER_H

#include <map>
#include <algorithm>
#include <string>
#include <fstream>

#include <gl/glew.h>
#include <gl/wglew.h>

using namespace std;

class tProgramManager
{
	protected:
		map<string, int> m_programHash;
		static GLchar* FileToString(ifstream*);
		static tProgramManager* m_instance;
	public:
		tProgramManager();
		virtual ~tProgramManager() { m_instance = NULL; };
		static tProgramManager* GetProgramManager();
		void AddProgram(string, int); //associate string with id
		int CreateProgram(string = "", string = "", string = ""); //returns the id of the newly created program
		int GetProgram(string);
};

#endif