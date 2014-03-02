#ifndef TOBJECT_H
#define TOBJECT_H

class tObject
{
public:
	virtual void Handler(int, bool) {};
	virtual void MouseHandler(int, int, const int&, const int&) { };
	virtual void Update() {};
};

#endif