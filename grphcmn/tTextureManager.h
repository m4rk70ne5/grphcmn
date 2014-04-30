#ifndef TTEXTUREMANAGER_H
#define TTEXTUREMANAGER_H

#include <map>
#include <algorithm>
#include <string>

using namespace std;

typedef unsigned char byte;

// targa specific defines
#define TGA_RGB		 2		// rgb file
#define TGA_A		 3		// alpha
#define TGA_RLE		10		// run-length encoded

// This is our image structure for our targa data
struct tImage
{
	int channels;			// # channels in the image (3 = RGB : 4 = RGBA)
	int sizeX;				// width
	int sizeY;				// height
	unsigned char *data;	// pixel data
};

class tTextureManager
{
protected:
	map<string, int> m_textureHash; //the int is the texture ID, the string is the name
	static tTextureManager* m_instance; //singleton
	tImage* LoadTGA(const char*);
public:
	tTextureManager();
	~tTextureManager();
	static tTextureManager* GetTextureManager();
	bool AddTexture(string, string, string); //string will usually be external texture file name
	int GetTexture(string); //return the ID of the texture name
};

#endif