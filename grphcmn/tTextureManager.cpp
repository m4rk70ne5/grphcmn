#include "tTextureManager.h"
#include "SOIL.h"
#include <fstream>
#include <gl/glew.h>
#include <gl/wglew.h>

tTextureManager* tTextureManager::m_instance = NULL; //definition of static member

tTextureManager::tTextureManager()
{
}

tTextureManager* tTextureManager::GetTextureManager()
{
	if (m_instance == NULL)
		m_instance = new tTextureManager;
	return m_instance;
}

tTextureManager::~tTextureManager()
{
	m_instance = NULL;
}

tImage* tTextureManager::LoadTGA(const char *strFileName)
{
	tImage *pImageData = NULL;			// This stores our important image data
	WORD width = 0, height = 0;			// The dimensions of the image
	byte length = 0;					// The length in bytes to the pixels
	byte imageType = 0;					// The image type (RLE, RGB, Alpha...)
	byte bits = 0;						// The bits per pixel for the image (16, 24, 32)
	FILE *pFile = NULL;					// The file pointer
	int channels = 0;					// The channels of the image (3 = RGA : 4 = RGBA)
	int stride = 0;						// The stride (channels * width)
	int i = 0;							// A counter

	// Open a file pointer to the targa file and check if it was found and opened 
	if((pFile = fopen(strFileName, "rb")) == NULL) 
	{
		return NULL;
	}
		
	// Allocate the structure that will hold our eventual image data (must free it!)
	pImageData = (tImage*)malloc(sizeof(tImage));

	// Read in the length in bytes from the header to the pixel data
	fread(&length, sizeof(byte), 1, pFile);
	
	// Jump over one byte
	fseek(pFile,1,SEEK_CUR); 

	// Read in the imageType (RLE, RGB, etc...)
	fread(&imageType, sizeof(byte), 1, pFile);
	
	// Skip past general information we don't care about
	fseek(pFile, 9, SEEK_CUR); 

	// Read the width, height and bits per pixel (16, 24 or 32)
	fread(&width,  sizeof(WORD), 1, pFile);
	fread(&height, sizeof(WORD), 1, pFile);
	fread(&bits,   sizeof(byte), 1, pFile);
	
	// Now we move the file pointer to the pixel data
	fseek(pFile, length + 1, SEEK_CUR); 

	// Check if the image is RLE compressed or not
	if(imageType != TGA_RLE)
	{
		// Check if the image is a 24 or 32-bit image
		if(bits == 24 || bits == 32)
		{
			// Calculate the channels (3 or 4) - (use bits >> 3 for more speed).
			// Next, we calculate the stride and allocate enough memory for the pixels.
			channels = bits / 8;
			stride = channels * width;
			pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*stride*height));

			// Load in all the pixel data line by line
			for(int y = 0; y < height; y++)
			{
				// Store a pointer to the current line of pixels
				unsigned char *pLine = &(pImageData->data[stride * y]);

				// Read in the current line of pixels
				fread(pLine, stride, 1, pFile);
			
				// Go through all of the pixels and swap the B and R values since TGA
				// files are stored as BGR instead of RGB (or use GL_BGR_EXT verses GL_RGB)
				for(i = 0; i < stride; i += channels)
				{
					int temp     = pLine[i];
					pLine[i]     = pLine[i + 2];
					pLine[i + 2] = temp;
				}
			}
		}
		// Check if the image is a 16 bit image (RGB stored in 1 unsigned short)
		else if(bits == 16)
		{
			unsigned short pixels = 0;
			int r=0, g=0, b=0;

			// Since we convert 16-bit images to 24 bit, we hardcode the channels to 3.
			// We then calculate the stride and allocate memory for the pixels.
			channels = 3;
			stride = channels * width;
			pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*stride*height));

			// Load in all the pixel data pixel by pixel
			for(int i = 0; i < width*height; i++)
			{
				// Read in the current pixel
				fread(&pixels, sizeof(unsigned short), 1, pFile);
				
				// Convert the 16-bit pixel into an RGB
				b = (pixels & 0x1f) << 3;
				g = ((pixels >> 5) & 0x1f) << 3;
				r = ((pixels >> 10) & 0x1f) << 3;
				
				// This essentially assigns the color to our array and swaps the
				// B and R values at the same time.
				pImageData->data[i * 3 + 0] = r;
				pImageData->data[i * 3 + 1] = g;
				pImageData->data[i * 3 + 2] = b;
			}
		}	
		// Else return a NULL for a bad or unsupported pixel format
		else
			return NULL;
	}
	// Else, it must be Run-Length Encoded (RLE)
	else
	{
		// Create some variables to hold the rleID, current colors read, channels, & stride.
		byte rleID = 0;
		int colorsRead = 0;
		channels = bits / 8;
		stride = channels * width;

		// Next we want to allocate the memory for the pixels and create an array,
		// depending on the channel count, to read in for each pixel.
		pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*stride*height));
		byte *pColors = ((byte*)malloc(sizeof(byte)*channels));

		// Load in all the pixel data
		while(i < width*height)
		{
			// Read in the current color count + 1
			fread(&rleID, sizeof(byte), 1, pFile);
			
			// Check if we don't have an encoded string of colors
			if(rleID < 128)
			{
				// Increase the count by 1
				rleID++;

				// Go through and read all the unique colors found
				while(rleID)
				{
					// Read in the current color
					fread(pColors, sizeof(byte) * channels, 1, pFile);

					// Store the current pixel in our image array
					pImageData->data[colorsRead + 0] = pColors[2];
					pImageData->data[colorsRead + 1] = pColors[1];
					pImageData->data[colorsRead + 2] = pColors[0];

					// If we have a 4 channel 32-bit image, assign one more for the alpha
					if(bits == 32)
						pImageData->data[colorsRead + 3] = pColors[3];

					// Increase the current pixels read, decrease the amount
					// of pixels left, and increase the starting index for the next pixel.
					i++;
					rleID--;
					colorsRead += channels;
				}
			}
			// Else, let's read in a string of the same character
			else
			{
				// Minus the 128 ID + 1 (127) to get the color count that needs to be read
				rleID -= 127;

				// Read in the current color, which is the same for a while
				fread(pColors, sizeof(byte) * channels, 1, pFile);

				// Go and read as many pixels as are the same
				while(rleID)
				{
					// Assign the current pixel to the current index in our pixel array
					pImageData->data[colorsRead + 0] = pColors[2];
					pImageData->data[colorsRead + 1] = pColors[1];
					pImageData->data[colorsRead + 2] = pColors[0];

					// If we have a 4 channel 32-bit image, assign one more for the alpha
					if(bits == 32)
						pImageData->data[colorsRead + 3] = pColors[3];

					// Increase the current pixels read, decrease the amount
					// of pixels left, and increase the starting index for the next pixel.
					i++;
					rleID--;
					colorsRead += channels;
				}
				
			}
				
		}
	}

	// Close the file pointer that opened the file
	fclose(pFile);

	// Fill in our tImage structure to pass back
	pImageData->channels = channels;
	pImageData->sizeX    = width;
	pImageData->sizeY    = height;

	// Return the TGA data (remember, you must free this data after you are done)
	return pImageData;
}

bool tTextureManager::AddTexture(string original_string, string texture_string, string extension)
{
	// check to see if the file exists
	ifstream file;
	file.open(texture_string);
	bool exists = file.good();
	file.close();
	
	if (exists)
	{
		GLuint texture_ID;
		//now load the texture with SOIL
		// SOIL can't work directly with modern OpenGL, so we can only 
		// load the raw image 
		tImage* image;
		if (!extension.compare("tga"))
			image = LoadTGA(texture_string.c_str());

		// change the width and height to powers of 2
		int new_width = 1, new_height = 1;
		while (new_width <= image->sizeX)
			new_width <<= 1;
		while (new_height <= image->sizeY)
			new_height <<= 1;
		//generate the texture
		glGenTextures(1, &texture_ID);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glBindTexture(GL_TEXTURE_2D, texture_ID);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->sizeX, image->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, image->data);
		
		// free the memory
		if (image)
		{
			if (image->data)
				free(image->data);
			free(image);
		}

		//store texture if successful
		if (texture_ID == 0)
			return false;
		m_textureHash[original_string] = texture_ID;

		//set the default border color
		//float defaultColor[4] = {0.0f, 0.0f, 0.0f, 1.0f}; //black
		// Typical Texture Generation Using Data From The Bitmap
		//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, defaultColor);
		// Build Mipmaps (builds different versions of the picture for distances - looks better)
		
		//Assign the mip map levels and texture info
		// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // for bsp's, they repeat.  in the future, 
		glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // make options available upon creation of texture
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		// When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		// Generate mipmaps, by the way.
		glGenerateMipmap(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0); //unbind the texture
		return true;
	}
	else
	{
		m_textureHash[original_string] = 0;
		return false;
	}
}

int tTextureManager::GetTexture(string textureName)
{
	return m_textureHash[textureName];
}