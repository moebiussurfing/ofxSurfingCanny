#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	canny.setup();

	// Load image
	img.load("TheDavid.png");
}

//--------------------------------------------------------------
void ofApp::update() {
	// Update processes the texture when needed
	canny.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(0);

	// Method 1: Using begin/end workflow (captures drawing commands)
	canny.begin();
	img.draw(0, 0);
	canny.end();

	// Draw processed output
	// Full screen
	canny.draw();

	// Or with custom size:
	// canny.draw(0, 0, ofGetWidth(), ofGetHeight());

	// Or get texture for custom rendering:
	// canny.getTexture().draw(x, y, w, h);

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

	// Alternative Method 2: Direct texture processing (without begin/end)
	// Just process the image texture directly:
	// canny.processTexture(img.getTexture());
	// canny.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::exit() {
	canny.exit();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	canny.keyPressed(key);
}
