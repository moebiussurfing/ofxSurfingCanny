#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// Load image first
	img.load("TheDavid.png");

	// Setup canny with image dimensions
	canny.setup(img.getWidth(), img.getHeight());

	// Or use default window size:
	// canny.setup();

	ofLogNotice("ofApp") << "Image size: " << img.getWidth() << "x" << img.getHeight();
	ofLogNotice("ofApp") << "Canny size: " << canny.getWidth() << "x" << canny.getHeight();
}

//--------------------------------------------------------------
void ofApp::update() {

	// Draw in between begin/end workflow
	canny.begin();
	img.draw(0, 0);
	canny.end();

	// Update processes the texture
	canny.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(128);

	// Draw processed output - full screen
	canny.draw();

	// Or with custom size:
	// canny.draw(0, 0, ofGetWidth(), ofGetHeight());

	// Or get texture for custom rendering:
	// canny.getTexture().draw(x, y, w, h);

	//--

	// UI

	// Draw original image thumbnail
	ofPushStyle();
	float r = img.getHeight() / (float)img.getWidth();
	int w = 100;
	int x = ofGetWidth() - w - 10;
	int y = 10;
	img.draw(x, y, w, w * r);
	ofNoFill();
	ofSetColor(255);
	ofDrawRectangle(x, y, w, w * r);
	ofPopStyle();

	// Draw GUI
	canny.drawGui();
}

//--------------------------------------------------------------
void ofApp::exit() {
	canny.exit();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == 'g' || key == 'G') {
		canny.toggleGui();
	}
	if (key == 'e' || key == 'E') {
		canny.toggleEnable();
	}
}
