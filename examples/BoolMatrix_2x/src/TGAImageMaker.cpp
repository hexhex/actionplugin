#include "../examples/BoolMatrix_2x/include/TGAImageMaker.h"

//Default Constructor
TGAImageMaker::TGAImageMaker() {

}

//Overridden Constructor
TGAImageMaker::TGAImageMaker(unsigned int width, unsigned int height) {
	m_width = width;
	m_height = height;
	m_pixels = new Colour[m_width * m_height];
}

//Set indivdual pixels
void TGAImageMaker::setPixel(Colour inputcolor, unsigned int x,
		unsigned int y) {
	m_pixels[convert2dto1d(x, y)] = inputcolor;
}

//Convert 2d array indexing to 1d indexing
unsigned int TGAImageMaker::convert2dto1d(unsigned int x, unsigned int y) {
	return m_width * x + y;
}

void TGAImageMaker::WriteImage(string filename) {

	//Error checking
	if (m_width <= 0 || m_height <= 0) {
		cout << "Image size is not set properly" << endl;
		return;
	}

	ofstream o(filename.c_str(), ios::out | ios::binary);

	//Write the header
	o.put(0);
	o.put(0);
	o.put(2); /* uncompressed RGB */
	o.put(0);
	o.put(0);
	o.put(0);
	o.put(0);
	o.put(0);
	o.put(0);
	o.put(0); /* X origin */
	o.put(0);
	o.put(0); /* y origin */
	o.put((m_width & 0x00FF));
	o.put((m_width & 0xFF00) / 256);
	o.put((m_height & 0x00FF));
	o.put((m_height & 0xFF00) / 256);
	o.put(32); /* 24 bit bitmap */
	o.put(0);

	//Write the pixel data
	for (unsigned int i = 0; i < m_height * m_width; i++) {
		o.put(m_pixels[i].b);
		o.put(m_pixels[i].g);
		o.put(m_pixels[i].r);
		o.put(m_pixels[i].a);
	}

	//close the file
	o.close();

}
