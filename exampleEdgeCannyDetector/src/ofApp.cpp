#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	auto t = ofGetElapsedTimeMillis();

	edgeDetector.setup();
	img.load("TheDavid.png");

	ofLogNotice("ofApp") << "Processing time (ms): " << (ofGetElapsedTimeMillis() - t);
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	auto t = ofGetElapsedTimeMillis();

	edgeDetector.begin();
	img.draw(0, 0);
	edgeDetector.end();

	if (ofGetFrameNum() % 120 == 0) ofLogNotice("ofApp") << "Processing time (ms): " << (ofGetElapsedTimeMillis() - t);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
}
