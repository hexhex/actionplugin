#ifndef TGA_IMAGE_MAKER_H
#define TGA_IMAGE_MAKER_H

//includes
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

//data structures
struct Colour {
	unsigned char r, g, b, a;
};

class TGAImageMaker {

public:

	//Constructor
	TGAImageMaker();

	//Overridden Constructor
	TGAImageMaker(unsigned int width, unsigned int height);

	//set individual pixels
	void setPixel(Colour inputcolor, unsigned int xposition,
			unsigned int yposition);

	void WriteImage(string filename);

private:

	//store the pixels
	Colour *m_pixels;

	unsigned int m_height;
	unsigned int m_width;

	//convert 2D to 1D indexing
	unsigned int convert2dto1d(unsigned int x, unsigned int y);

};

#endif /* TGA_IMAGE_MAKER_H */
