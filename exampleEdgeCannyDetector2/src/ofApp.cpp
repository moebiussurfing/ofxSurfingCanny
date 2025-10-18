#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetWindowTitle("Canny Edge Detector - Real Time");
	ofSetFrameRate(60);

	// Setup edge detector
	edgeDetector.setup();

	// Load image
	img.load("TheDavid.png");

	// Setup GUI
	gui.setup("Controls", "settings.xml", 10, 10);
	gui.add(edgeDetector.parameters);
	showGui = true;

	// Load saved settings
	edgeDetector.loadSettings("canny_settings.json");

	ofLogNotice("ofApp") << "Setup complete. Press 'g' to toggle GUI, 's' to save settings";
}

//--------------------------------------------------------------
void ofApp::update() {
	// Parameters are automatically updated through ofParameter system
	// No need to manually update anything
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(0);

	// Process image in real-time with current parameters
	edgeDetector.begin();
	img.draw(0, 0);
	edgeDetector.end();

	// Draw GUI
	if (showGui) {
		gui.draw();

		// Draw info text
		ofSetColor(255);
		string info = "FPS: " + ofToString(ofGetFrameRate(), 1) + "\n";
		info += "Press 'g' to toggle GUI\n";
		info += "Press 's' to save settings\n";
		info += "Press 'l' to load settings";
		ofDrawBitmapString(info, 10, ofGetHeight() - 80);
	}
}

//--------------------------------------------------------------
void ofApp::exit() {
	// Auto-save settings on exit
	edgeDetector.saveSettings("canny_settings.json");
	ofLogNotice("ofApp") << "Settings auto-saved on exit";
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
	case 'g':
	case 'G':
		showGui = !showGui;
		break;

	case 's':
	case 'S':
		edgeDetector.saveSettings("canny_settings.json");
		ofLogNotice("ofApp") << "Settings saved manually";
		break;

	case 'l':
	case 'L':
		edgeDetector.loadSettings("canny_settings.json");
		ofLogNotice("ofApp") << "Settings loaded manually";
		break;
	}
}
