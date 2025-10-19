#pragma once

#include "ofMain.h"
#include "ofxSurfingCanny.h"

class ofApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);

	ofxSurfingCanny canny;
	ofImage img;
};
