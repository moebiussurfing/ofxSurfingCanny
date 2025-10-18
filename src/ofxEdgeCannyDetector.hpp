//
//  ofxEdgeCannyDetector.hpp
//  example
//
//  Created by Pierre Tardif on 15/05/2020.
//

#ifndef ofxEdgeCannyDetector_hpp
#define ofxEdgeCannyDetector_hpp

#include "ofMain.h"
#include <stdio.h>

class ofxEdgeCannyDetector {

public:
	ofxEdgeCannyDetector() {
	}

	ofShader eDetector, gaussian;
	glm::vec2 size;
	ofFbo f;

	// Parameters
	ofParameterGroup parameters;
	ofParameter<float> highThreshold;
	ofParameter<float> lowThreshold;
	ofParameter<float> gradientScale;
	ofParameter<bool> useGaussian;

	void setup() {
		gaussian.load("../../../../../addons/ofxEdgeCannyDetector/src/Shaders/gaussian");
		eDetector.load("../../../../../addons/ofxEdgeCannyDetector/src/Shaders/sobel");

		size = glm::vec2(ofGetWidth(), ofGetHeight());
		f.allocate(size.x, size.y);

		// Setup parameters
		parameters.setName("Canny Edge Detector");
		parameters.add(highThreshold.set("High Threshold", 0.35, 0.0, 1.0));
		parameters.add(lowThreshold.set("Low Threshold", 0.1, 0.0, 1.0));
		parameters.add(gradientScale.set("Gradient Scale", 1.0, 0.1, 5.0));
		parameters.add(useGaussian.set("Use Gaussian", true));
	}

	void begin() {
		f.begin();
		ofClear(0);

		if (useGaussian) {
			gaussian.begin();
		}
	}

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

	void loadSettings(const string & filename = "canny_settings.json") {
		ofJson json = ofLoadJson(filename);
		if (!json.empty()) {
			if (json.contains("highThreshold")) highThreshold = json["highThreshold"];
			if (json.contains("lowThreshold")) lowThreshold = json["lowThreshold"];
			if (json.contains("gradientScale")) gradientScale = json["gradientScale"];
			if (json.contains("useGaussian")) useGaussian = json["useGaussian"];
			ofLogNotice("ofxEdgeCannyDetector") << "Settings loaded from " << filename;
		}
	}

	void saveSettings(const string & filename = "canny_settings.json") {
		ofJson json;
		json["highThreshold"] = highThreshold.get();
		json["lowThreshold"] = lowThreshold.get();
		json["gradientScale"] = gradientScale.get();
		json["useGaussian"] = useGaussian.get();
		ofSavePrettyJson(filename, json);
		ofLogNotice("ofxEdgeCannyDetector") << "Settings saved to " << filename;
	}
};

#endif /* ofxEdgeCannyDetector_hpp */
