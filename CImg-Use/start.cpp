
#define cimg_use_png
#include "CImg/CImg.h"

using namespace cimg_library;

  int main0() {
    CImg<unsigned char> img(920,920,1,4,100);        // Define a 640x400 color image with 8 bits per color component.
	const unsigned char start[] = {0,0,0,100};
	//img.fill(100);                                 // Set pixel values to 0 (color : black)
    unsigned char purple[] = { 255,0,255, 255 };      // Define a purple color
    img.draw_text(100,100,"Hello World",purple); // Draw a purple "Hello world" at coordinates (100,100).
    img.display("My first CImg code");           // Display the image in a display window.
	img.draw_line(20,20,100,100,purple);
	img.save("a.png");
    return 0;
  }