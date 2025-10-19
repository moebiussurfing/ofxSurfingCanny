//
//  ofxEdgeCannyDetector.hpp
//  example
//
//  Created by Pierre Tardif on 15/05/2020.
//  Modified by moebiusSurfing 19/10/2025
//

#ifndef ofxEdgeCannyDetector_hpp
#define ofxEdgeCannyDetector_hpp

#include "ofMain.h"
#include "ofxGui.h"
#include <stdio.h>

class ofxEdgeCannyDetector {

public:

	ofxEdgeCannyDetector() {
		ofLogNotice("ofxEdgeCannyDetector") << "ofxEdgeCannyDetector()";
	}

private:

	ofShader eDetector, gaussian;
	glm::vec2 size;
	ofFbo f;

public:

	ofParameterGroup parameters; // Preset settings
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
	ofEventListener e_bAutoSaveLoad; // required to improve user workflow

	bool bEnableSettings = true; // To disable settings saving/loading (ie: handled in ofApp)

public:

	// GUI
	ofxPanel gui;

	//----------------------------------
	void setDisableSettings(bool bDisable) { // To disable settings saving/loading (ie: handled in ofApp)
		ofLogNotice("ofxEdgeCannyDetector") << "setDisableSettings() bEnableSettings: " << bEnableSettings;

		bEnableSettings = !bDisable;
	}

private:

	// Default values
	const float DEFAULT_HIGH_THRESHOLD = 0.35f;
	const float DEFAULT_LOW_THRESHOLD = 0.1f;
	const float DEFAULT_GRADIENT_SCALE = 1.0f;
	const bool DEFAULT_USE_GAUSSIAN = true;
	const bool DEFAULT_AUTO_SAVE = true;

	// `bin/data/path_folder` folder containering settings files
	std::string path_folder = "ofxEdgeCannyDetector";

public:

	//----------------------------------
	void setup() {
		ofLogNotice("ofxEdgeCannyDetector") << "setup()";

		// Load shaders
		// TODO: move to /bin/data/shaders/...
		gaussian.load("../../../../../addons/ofxEdgeCannyDetector/src/Shaders/gaussian");
		eDetector.load("../../../../../addons/ofxEdgeCannyDetector/src/Shaders/sobel");

		size = glm::vec2(ofGetWidth(), ofGetHeight());
		f.allocate(size.x, size.y);

		setupParameters();
		setupCallbacks();
		setupGui();
		startup();

		ofLogNotice("ofxEdgeCannyDetector") << "setup() complete.";
		ofLogNotice("ofxEdgeCannyDetector") << "Press 'g' to toggle GUI";
	}

private:

	//----------------------------------
	void setupParameters() {
		ofLogNotice("ofxEdgeCannyDetector") << "setupParameters()";

		// Setup parameters with default values
		parameters.setName("Canny Edge");
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
		ofLogNotice("ofxEdgeCannyDetector") << "setupCallbacks()";

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
	}

	//----------------------------------
	void setupGui() {
		ofLogNotice("ofxEdgeCannyDetector") << "setupGui()";

		gui.setup("ofxEdgeCannyDetector");
		gui.add(parameters);
		gui.add(parametersSession);

		bGui = true;
	}

	//----------------------------------
	void startup() {
		ofLogNotice("ofxEdgeCannyDetector") << "startup()";

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
		ofLogNotice("ofxEdgeCannyDetector") << "resetToDefaults()";

		highThreshold = DEFAULT_HIGH_THRESHOLD;
		lowThreshold = DEFAULT_LOW_THRESHOLD;
		gradientScale = DEFAULT_GRADIENT_SCALE;
		useGaussian = DEFAULT_USE_GAUSSIAN;
	}

	//----------------------------------
	void begin() {
		f.begin();
		ofClear(0);

		if (useGaussian) {
			gaussian.begin();
		}
	}

	//----------------------------------
	void end() {
		if (useGaussian) {
			gaussian.end();
		}
		f.end();

		eDetector.begin();
		eDetector.setUniform2f("u_resImg", size);
		eDetector.setUniform1f("u_highThreshold", highThreshold);
		eDetector.setUniform1f("u_lowThreshold", lowThreshold);
		eDetector.setUniform1f("u_gradientScale", gradientScale);
		eDetector.setUniform1i("u_useGaussian", useGaussian ? 1 : 0);
		f.draw(0, 0);
		eDetector.end();
	}

	//----------------------------------
	void draw() {
		if (bGui) {
			gui.draw();

			// Draw info text
			std::string info = "Press 'g' to toggle GUI\n";
			info += "FPS: " + ofToString(ofGetFrameRate(), 0);
			ofDrawBitmapStringHighlight(info, 10, ofGetHeight() - 40);
		}
	}

	//--

	//----------------------------------
	void toggleGui() {
		ofLogNotice("ofxEdgeCannyDetector") << "toggleGui() Settings auto-saved on exit";

		bGui = !bGui;
	}

	//----------------------------------
	void keyPressed(int key) {
		if (key == 'g' || key == 'G') {
			toggleGui();
		}
	}

	//----------------------------------
	void exit() {
		ofLogNotice("ofxEdgeCannyDetector") << "exit()";

		// Auto-save on exit if enabled
		if (bAutoSaveLoad) {
			ofLogNotice("ofxEdgeCannyDetector") << "exit() Settings auto-saved on exit";
			save(parameters);
			save(parametersSession);
		}

		// No need to manually remove listeners when using ofEventListener objects
	}

	//--

private:

	//----------------------------------
	void load(ofParameterGroup & parameters_, std::string _path_Folder = "") {
		ofLogNotice("ofxEdgeCannyDetector") << "Load: " << parameters_.getName();

		if (_path_Folder.empty()) _path_Folder = path_folder;
		// Usar ofFilePath::join para construir el path relativo a /bin/data/
		std::string filename = parameters_.getName() + ".json";
		std::string relPath = ofFilePath::join(_path_Folder, filename);
		std::string dataPath = ofToDataPath(relPath, true); // true = relative to data folder
		ofLogNotice("ofxEdgeCannyDetector") << "dataPath: " << dataPath;

		loadSettings(parameters_, dataPath);
	}

	//----------------------------------
	void save(ofParameterGroup & parameters_, std::string _path_Folder = "") {
		ofLogNotice("ofxEdgeCannyDetector") << "Save: " << parameters_.getName();

		if (_path_Folder.empty()) _path_Folder = path_folder;
		std::string filename = parameters_.getName() + ".json";
		std::string relPath = ofFilePath::join(_path_Folder, filename);
		std::string dataPath = ofToDataPath(relPath, true);
		ofLogNotice("ofxEdgeCannyDetector") << "dataPath: " << dataPath;

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

#endif /* ofxEdgeCannyDetector_hpp */
