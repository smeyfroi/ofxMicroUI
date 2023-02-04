#pragma once

class ofxMicroUISoftware { // : public ofBaseApp
public:
//	ofKey OF_KEY_SAVE = OF_KEY_SUPER;
//	ofKey OF_KEY_SAVE = OF_KEY_ALT;
	// 2021 - software name
	ofxMicroUI * _ui = NULL;
	
	bool showFbo = true;
	glm::vec2 windowSize = glm::vec2(1280,720);

	string name = "";

	ofFbo fbo, fbo2, fbo3;
	ofFbo * fboFinal = &fbo;

	ofPixels fboPixels;
	ofRectangle fboRect;
	ofRectangle fboRectFull;

#if defined(TARGET_RASPBERRY_PI) || defined(TARGET_OF_IOS)
	int depth = GL_RGBA;
#else
	//GL_RGBA GL_RGBA16F GL_RGBA32F
	int depth = GL_RGBA16F;
#endif
	/*
	 GL_RGB16F or GL_RGBA16F
	 GL_RGB32F and GL_RGBA32F
	 
	 GL_RGBA32F
	 GL_RGBA32F_ARB
	 GL_RGBA64F
	 */
	
	bool usingSoftScroll = true;
	int softW = 500;
	int softScroll = 0;
	bool usePresetShortcut = true;
	
	bool dragging = false;
	bool dragFbo = false;
	glm::vec2 firstXY;

	struct drag {
		ofxMicroUI::element *ex = NULL;
		ofxMicroUI::element *ey = NULL;
		glm::vec2 dragPos;
		void update(glm::vec2 xy) {
			dragPos = xy;
			if (ex != NULL) {
				ex->set(xy.x);
			}
			if (ey != NULL) {
				ey->set(xy.y);
			}
		}
	};
	
	ofxMicroUISoftware();
	ofxMicroUISoftware(ofxMicroUI * u, string n, ofFbo * f);
	ofxMicroUISoftware(ofxMicroUI * u, string n, int nFbos = 1);
	void setup();
	void setupFromText(string fileName, int line = 0);
	void afterSetUI();
	void addControlUI(ofxMicroUI * _ui);
	void setUI(ofxMicroUI * u);
	void updateFboRect();
	void drawFbo();
	void allocateFbos(int w, int h, int multiSampling = 0);
	void loadPreset(string s);
	void keyPressed(int key);
	
	void uiEventMaster(string & e);
	void uiEventsAll(ofxMicroUI::element & e);
	void uiEvents(ofxMicroUI::element & e); 	// Master only
	
	void onKeyPressed(ofKeyEventArgs & data);
	void onMousePressed(ofMouseEventArgs & data);
	void onMouseDragged(ofMouseEventArgs & data);
	void onMouseReleased(ofMouseEventArgs & data);
	void onExit(ofEventArgs & data);
	

	ofShortPixels shortPixelsExport;
	ofPixels pixelsExport;
	ofFbo fboExport;

	void fboToPixels(bool useShort = true);
	void fboToPng();

#ifdef USETIFFFASTWRITER
#include "tiffFastWriter.h"
	
	void fboToTiff() {
		fboToPixels(false);
		string p = ofToString(_ui->pString["presets"]);
		string folder = "_output";
		if (!ofFile::doesFileExist(folder)) {
			ofDirectory::createDirectory(folder);
		}
		// create directory if doesnt exist
		string fullFileName = folder + "/" + p + "_" +ofGetTimestampString() + ".tif";
		tiffFastWriter rec;
		rec.recordTiff(&pixelsExport, fullFileName);
		string resultado = ofSystem("open " + ofxMicroUI::dataPath(fullFileName));
	}
	
	void fboToTiff(string n) {
		fboToPixels();
		tiffFastWriter rec;
		rec.recordTiff(&pixelsExport, n);
	}
#endif
};
