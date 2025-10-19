//
//  ofxSurfingCanny.h
//  example
//
//  Created by Pierre Tardif on 15/05/2020.
//  Modified by moebiusSurfing 19/10/2025
//

#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include <stdio.h>

class ofxSurfingCanny {

public:
	ofxSurfingCanny() {
		ofLogNotice("ofxSurfingCanny") << "ofxSurfingCanny()";
	}

private:
	ofShader eDetector, gaussian;
	glm::vec2 size;
	ofFbo fboInput; // Input texture for processing
	ofFbo fboOutput; // Processed output
	ofFbo fboGaussian; // Temporary FBO for gaussian pass
	ofTexture * texInput; // Pointer to input texture

	bool bNeedUpdate = false;

public:
	ofParameterGroup parameters; // Preset settings
	ofParameter<bool> bEnable;
	ofParameter<float> highThreshold;
	ofParameter<float> lowThreshold;
	ofParameter<float> gradientScale;
	ofParameter<bool> useGaussian;
	ofParameter<void> vResetButton;

	ofParameterGroup parametersSettingsManager; // Settings management
	ofParameter<bool> bAutoSaveLoad;
	ofParameter<void> vSaveButton;
	ofParameter<void> vLoadButton;

	ofParameterGroup parametersSession; // Session settings
	ofParameter<bool> bGui;

private:
	// Lambda event listeners
	ofEventListener e_vResetButton;
	ofEventListener e_vSaveButton;
	ofEventListener e_vLoadButton;
	ofEventListener e_bAutoSaveLoad;
	ofEventListener e_bEnable;

	bool bEnableSettings = true; // To disable settings saving/loading (ie: handled in ofApp)

public:
	// GUI
	ofxPanel gui;

	//----------------------------------
	void setDisableSettings(bool bDisable) {
		ofLogNotice("ofxSurfingCanny") << "setDisableSettings() bEnableSettings: " << bEnableSettings;
		bEnableSettings = !bDisable;
	}

private:
	// Default values
	const bool DEFAULT_ENABLE = true;
	const float DEFAULT_HIGH_THRESHOLD = 0.35f;
	const float DEFAULT_LOW_THRESHOLD = 0.1f;
	const float DEFAULT_GRADIENT_SCALE = 1.0f;
	const bool DEFAULT_USE_GAUSSIAN = true;
	const bool DEFAULT_AUTO_SAVE = true;

	// `bin/data/path_folder` folder containing settings files
	std::string path_folder = "ofxSurfingCanny";

public:
	//----------------------------------
	void setup() {
		setup(ofGetWidth(), ofGetHeight());
	}

	//----------------------------------
	void setup(int width, int height) {
		ofLogNotice("ofxSurfingCanny") << "setup(" << width << ", " << height << ")";

		// Load shaders
		// TODO: move to /bin/data/shaders/...
		gaussian.load("../../../../../addons/ofxSurfingCanny/src/Shaders/gaussian");
		eDetector.load("../../../../../addons/ofxSurfingCanny/src/Shaders/sobel");

		allocate(width, height);

		setupParameters();
		setupCallbacks();
		setupGui();
		startup();

		ofLogNotice("ofxSurfingCanny") << "setup() complete.";
		ofLogNotice("ofxSurfingCanny") << "Press 'g' to toggle GUI, 'e' to toggle Enable";
	}

private:
	//----------------------------------
	void allocate(int width, int height) {
		ofLogNotice("ofxSurfingCanny") << "allocate(" << width << ", " << height << ")";

		size = glm::vec2(width, height);

		fboInput.allocate(size.x, size.y, GL_RGBA);
		fboOutput.allocate(size.x, size.y, GL_RGBA);
		fboGaussian.allocate(size.x, size.y, GL_RGBA);

		fboInput.begin();
		ofClear(0, 0, 0, 255);
		fboInput.end();

		fboOutput.begin();
		ofClear(0, 0, 0, 255);
		fboOutput.end();

		fboGaussian.begin();
		ofClear(0, 0, 0, 255);
		fboGaussian.end();
	}

public:
	//----------------------------------
	void setSize(int width, int height) {
		if (size.x != width || size.y != height) {
			ofLogNotice("ofxSurfingCanny") << "setSize(" << width << ", " << height << ")";
			allocate(width, height);
		}
	}

	//----------------------------------
	glm::vec2 getSize() {
		return size;
	}

	//----------------------------------
	int getWidth() {
		return size.x;
	}

	//----------------------------------
	int getHeight() {
		return size.y;
	}

private:
	//----------------------------------
	void setupParameters() {
		ofLogNotice("ofxSurfingCanny") << "setupParameters()";

		// Setup parameters with default values
		parameters.setName("Canny Edge");
		parameters.add(bEnable.set("Enable", DEFAULT_ENABLE));
		parameters.add(highThreshold.set("High Threshold", DEFAULT_HIGH_THRESHOLD, 0.0, 1.0));
		parameters.add(lowThreshold.set("Low Threshold", DEFAULT_LOW_THRESHOLD, 0.0, 1.0));
		parameters.add(gradientScale.set("Gradient Scale", DEFAULT_GRADIENT_SCALE, 0.1, 5.0));
		parameters.add(useGaussian.set("Gaussian", DEFAULT_USE_GAUSSIAN));
		parameters.add(vResetButton.set("Reset"));

		// Setup session settings
		parametersSession.setName("Session");
		parametersSettingsManager.setName("Settings");
		parametersSettingsManager.add(bAutoSaveLoad.set("Auto", DEFAULT_AUTO_SAVE));
		parametersSettingsManager.add(vSaveButton.set("Save"));
		parametersSettingsManager.add(vLoadButton.set("Load"));
		parametersSession.add(parametersSettingsManager);
		parametersSession.add(bGui.set(parameters.getName(), true));
	}

	//----------------------------------
	void setupCallbacks() {
		ofLogNotice("ofxSurfingCanny") << "setupCallbacks()";

		// Setup callbacks using lambda event listeners
		e_vResetButton = vResetButton.newListener([this]() {
			this->resetToDefaults();
		});
		e_vSaveButton = vSaveButton.newListener([this]() {
			this->save(parameters);
		});
		e_vLoadButton = vLoadButton.newListener([this]() {
			this->load(parameters);
		});
		e_bAutoSaveLoad = bAutoSaveLoad.newListener([this](bool & b) {
			this->save(parametersSession);
		});
		e_bEnable = bEnable.newListener([this](bool & b) {
			if (b) bNeedUpdate = true;
		});
	}

	//----------------------------------
	void setupGui() {
		ofLogNotice("ofxSurfingCanny") << "setupGui()";

		gui.setup("ofxSurfingCanny");
		gui.add(parameters);
		gui.add(parametersSession);

		auto & g = gui.getGroup(parametersSession.getName());
		g.getGroup(parametersSettingsManager.getName()).minimize();
		g.minimize();

		bGui = true;
	}

	//----------------------------------
	void startup() {
		ofLogNotice("ofxSurfingCanny") << "startup()";

		// Auto-load on startup if enabled
		if (bAutoSaveLoad && bEnableSettings) {
			load(parameters);
			load(parametersSession);
		} else {
			resetToDefaults();
		}
	}

public:
	//----------------------------------
	void resetToDefaults() {
		ofLogNotice("ofxSurfingCanny") << "resetToDefaults()";

		bEnable = DEFAULT_ENABLE;
		highThreshold = DEFAULT_HIGH_THRESHOLD;
		lowThreshold = DEFAULT_LOW_THRESHOLD;
		gradientScale = DEFAULT_GRADIENT_SCALE;
		useGaussian = DEFAULT_USE_GAUSSIAN;
	}

	//----------------------------------
	// API similar to ofxSurfingDepthMap
	void update() {
		if (!bEnable) return;
		if (!bNeedUpdate) return;

		processTexture();
		bNeedUpdate = false;
	}

	//----------------------------------
	void begin() {
		//if (!bEnable) return;

		fboInput.begin();
		ofClear(0, 0, 0, 255);
	}

	//----------------------------------
	void end() {
		//if (!bEnable) return;

		fboInput.end();
		texInput = &fboInput.getTexture();
		bNeedUpdate = true;
	}

private:
	//----------------------------------
	void processTexture() {
		if (texInput == nullptr) return;

		// Apply Gaussian if enabled
		if (useGaussian) {
			fboGaussian.begin();
			ofClear(0, 0, 0, 255);

			// Enable texture and bind it
			ofEnableArbTex();
			texInput->bind();

			gaussian.begin();
			gaussian.setUniform2f("u_resImg", size);

			// Draw a quad with correct texture coordinates for ARB_texture_rectangle
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(0, 0);
			glTexCoord2f(size.x, 0);
			glVertex2f(size.x, 0);
			glTexCoord2f(size.x, size.y);
			glVertex2f(size.x, size.y);
			glTexCoord2f(0, size.y);
			glVertex2f(0, size.y);
			glEnd();

			gaussian.end();
			texInput->unbind();
			fboGaussian.end();

			texInput = &fboGaussian.getTexture();
		}

		// Apply Sobel edge detection
		fboOutput.begin();
		ofClear(0, 0, 0, 255);

		// Enable texture and bind it
		ofEnableArbTex();
		texInput->bind();

		eDetector.begin();
		eDetector.setUniform2f("u_resImg", size);
		eDetector.setUniform1f("u_highThreshold", highThreshold);
		eDetector.setUniform1f("u_lowThreshold", lowThreshold);
		eDetector.setUniform1f("u_gradientScale", gradientScale);
		eDetector.setUniform1i("u_useGaussian", 0); // Always 0 here, gaussian is applied before

		// Draw a quad with correct texture coordinates for ARB_texture_rectangle
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(0, 0);
		glTexCoord2f(size.x, 0);
		glVertex2f(size.x, 0);
		glTexCoord2f(size.x, size.y);
		glVertex2f(size.x, size.y);
		glTexCoord2f(0, size.y);
		glVertex2f(0, size.y);
		glEnd();

		eDetector.end();
		texInput->unbind();
		fboOutput.end();
	}

public:
	//----------------------------------
	// Draw processed output (full size by default)
	void draw() {
		draw(0, 0, size.x, size.y);
	}

	//----------------------------------
	// Draw processed output at position with size
	void draw(float x, float y, float w, float h) {
		//if (!bEnable) return;

		if (bEnable)
			fboOutput.draw(x, y, w, h);
		else
			fboInput.draw(x, y, w, h);
	}

	//----------------------------------
	// Get output texture for custom rendering
	ofTexture & getTexture() {
		return fboOutput.getTexture();
	}

	//----------------------------------
	// Get output FBO
	ofFbo & getFbo() {
		return fboOutput;
	}

	//----------------------------------
	void drawGui() {
		if (bGui) {
			gui.draw();

			// Draw info text
			std::string info = "Press 'g' to toggle GUI, 'e' to toggle Enable\n";
			info += "FPS: " + ofToString(ofGetFrameRate(), 0);
			ofDrawBitmapStringHighlight(info, 10, ofGetHeight() - 40);
		}
	}

	//--

	//----------------------------------
	void toggleGui() {
		ofLogNotice("ofxSurfingCanny") << "toggleGui()";
		bGui = !bGui;
	}

	//----------------------------------
	void setEnableGui(bool enable) {
		bGui = enable;
	}

	//----------------------------------
	void keyPressed(int key) {
		if (key == 'g' || key == 'G') {
			toggleGui();
		}
		if (key == 'e' || key == 'E') {
			bEnable = !bEnable;
		}
	}

	//----------------------------------
	void exit() {
		ofLogNotice("ofxSurfingCanny") << "exit()";

		// Auto-save on exit if enabled
		if (bAutoSaveLoad && bEnableSettings) {
			ofLogNotice("ofxSurfingCanny") << "exit() Settings auto-saved on exit";
			save(parameters);
			save(parametersSession);
		}

		// No need to manually remove listeners when using ofEventListener objects
	}

	//--

private:
	//----------------------------------
	void load(ofParameterGroup & parameters_, std::string _path_Folder = "") {
		ofLogNotice("ofxSurfingCanny") << "Load: " << parameters_.getName();

		if (_path_Folder.empty()) _path_Folder = path_folder;
		std::string filename = parameters_.getName() + ".json";
		std::string relPath = ofFilePath::join(_path_Folder, filename);
		std::string dataPath = ofToDataPath(relPath, true);
		ofLogNotice("ofxSurfingCanny") << "dataPath: " << dataPath;

		loadSettings(parameters_, dataPath);
	}

	//----------------------------------
	void save(ofParameterGroup & parameters_, std::string _path_Folder = "") {
		ofLogNotice("ofxSurfingCanny") << "Save: " << parameters_.getName();

		if (_path_Folder.empty()) _path_Folder = path_folder;
		std::string filename = parameters_.getName() + ".json";
		std::string relPath = ofFilePath::join(_path_Folder, filename);
		std::string dataPath = ofToDataPath(relPath, true);
		ofLogNotice("ofxSurfingCanny") << "dataPath: " << dataPath;

		saveSettings(parameters_, dataPath);
	}

	//----

	// Taken from ofxSurfingHelpers

	// Load
	//--------------------------------------------------------------
	inline bool loadSettings(ofParameterGroup & parameters, std::string path = "") {
		if (path == "") path = parameters.getName() + ".json";

		//verify that the file exist
		ofFile f;
		bool b = f.doesFileExist(path);
		if (b)
			ofLogVerbose("ofxSurfing") << "Found settings file: " << path << " for ofParameterGroup: " << parameters.getName();
		else
			ofLogError("ofxSurfing") << "File " << path
									 << " for ofParameterGroup " << parameters.getName() << " not found!";
		//load if exist
		if (b) {
			ofJson settings;
			settings = ofLoadJson(path);
			ofDeserialize(settings, parameters);
		}

		return b;
	}

	// Save
	//--------------------------------------------------------------
	inline bool saveSettings(ofParameterGroup & parameters, std::string path = "") {
		if (path == "") path = parameters.getName() + ".json";

		//if the destination folder do not exists, will be created.
		if (!ofDirectory::doesDirectoryExist(ofFilePath::getEnclosingDirectory(path))) {
			if (ofFilePath::createEnclosingDirectory(path))
				ofLogWarning("ofxSurfing") << "Created enclosing folder for: " << path;
			else
				ofLogError("ofxSurfing") << "Unable to create enclosing folder for: " << path;
		}
		//try to save the file
		ofJson settings;
		ofSerialize(settings, parameters);
		bool b = ofSavePrettyJson(path, settings);
		if (b)
			ofLogVerbose("ofxSurfing") << "Saved ofParameterGroup: " << parameters.getName() << " to " << path;
		else
			ofLogError("ofxSurfing") << "Error saving: " << parameters.getName() << " to " << path;

		return b;
	}
};
