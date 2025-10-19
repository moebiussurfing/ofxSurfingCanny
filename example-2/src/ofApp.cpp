#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Canny Edge Detector - Real Time");
	ofSetFrameRate(60);

	// Setup edge detector (it handles everything internally)
	edgeDetector.setup();

	// Load image
	img.load("TheDavid.png");

	ofLogNotice("ofApp") << "Setup complete";
}

//--------------------------------------------------------------
void ofApp::update() {
	// Nothing to update - parameters are handled by the addon
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(0);

	// Process image in real-time with current parameters
	edgeDetector.begin();
	img.draw(0, 0);
	edgeDetector.end();

	// Draw addon GUI (handles everything internally)
	edgeDetector.draw();
}

//--------------------------------------------------------------
void ofApp::exit() {
	// Let the addon handle its own cleanup
	edgeDetector.exit();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// Forward key events to addon
	edgeDetector.keyPressed(key);
}
