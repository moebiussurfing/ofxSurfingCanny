#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	canny.setup();

	// Load image
	img.load("TheDavid.png");
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {

	// Process image in real-time with current parameters
	canny.begin();
	img.draw(0, 0);
	canny.end();

	// Draw original image
	ofPushStyle();
	float r = img.getHeight() / (float)img.getWidth();
	int w = 100;
	int x = ofGetWidth() - w;
	int y =  0;
	img.draw(x, y, w, w * r);
	ofNoFill();
	ofSetColor(255);
	ofDrawRectangle(x, y, w, w * r);
	ofPopStyle();

	canny.drawGui();
}

//--------------------------------------------------------------
void ofApp::exit() {
	canny.exit();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	canny.keyPressed(key);
}
