#include "ofxMicroUI.h"
#include "ofxMicroUISoftware.h"

using std::string;

map <char, int> keyPreset = {
	{ 'a', 0 },
	{ 's', 1 },
	{ 'd', 2 },
	{ 'f', 3 },
	{ 'g', 4 },
	{ 'h', 5 },
	{ 'j', 6 },
	{ 'k', 7 },
	{ 'l', 8 },
	{ 'z', 9 },
	{ 'x', 10 },
	{ 'c', 11 },
	{ 'v', 12 },
	{ 'b', 13 },
	{ 'n', 14 },
	{ 'm', 15 },
	{ ',', 16 },
	{ '.', 17 }
};

ofxMicroUISoftware::ofxMicroUISoftware() {
//		setup();
}

ofxMicroUISoftware::ofxMicroUISoftware(ofxMicroUI * u, string n, ofFbo * f) : _ui(u), name(n), fboFinal(f) {
	ofAddListener(_ui->uiEventMaster, this, &ofxMicroUISoftware::uiEventMaster);
	setup();
}

ofxMicroUISoftware::ofxMicroUISoftware(ofxMicroUI * u, string n, int nFbos) : _ui(u), name(n) {
	ofAddListener(_ui->uiEventMaster, this, &ofxMicroUISoftware::uiEventMaster);
	if (nFbos == 1) {
		fboFinal = &fbo;
	}
	else if (nFbos == 2) {
		fboFinal = &fbo2;
	}
	else if (nFbos == 3) {
		fboFinal = &fbo3;
	}

	_ui->_fboPreset = fboFinal;
	
	setup();
}

void ofxMicroUISoftware::setup() {
	ofAddListener(ofEvents().keyPressed, this, &ofxMicroUISoftware::onKeyPressed);
	ofAddListener(ofEvents().mousePressed, this, &ofxMicroUISoftware::onMousePressed);
	ofAddListener(ofEvents().mouseDragged, this, &ofxMicroUISoftware::onMouseDragged);
	ofAddListener(ofEvents().mouseReleased, this, &ofxMicroUISoftware::onMouseReleased);
	ofAddListener(ofEvents().exit, this, &ofxMicroUISoftware::onExit);
	setupFromText("_output.txt");
	
	windowSize = glm::vec2(ofGetWindowWidth(), ofGetWindowHeight());
}



void ofxMicroUISoftware::setupFromText(string fileName, int line) {
	int w = 0, h = 0, multiSampling = 0;
	if (ofFile::doesFileExist(fileName)) {
		vector <string> output = ofxMicroUI::textToVector(fileName);
		if (output.size()) {
			vector <string> dimensoes = ofSplitString(output[line], " ");
			w = ofToInt(dimensoes[0]);
			h = ofToInt(dimensoes[1]);
			if (dimensoes.size() > 2) {
				multiSampling = ofToInt(dimensoes[2]);
			}
		}
	} else {
		cout << "missing output.txt file : " << fileName << endl;
		w = 1280;
		h = 720;
	}
	allocateFbos(w, h, multiSampling);
	updateFboRect();
}


void ofxMicroUISoftware::afterSetUI() {
//		 cout << "****************************************************************" << endl;
//		 cout << "afterSetUI agora" << endl;
	string f = "_ui/_style.txt";
	if (ofFile::doesFileExist(f)) {
		_ui->_settings->styleLines = ofBufferFromFile(f).getText();
	}
	

	fboRectFull = ofRectangle(0,0,fboFinal->getWidth(), fboFinal->getHeight());
	ofAddListener(_ui->uiEvent, this, &ofxMicroUISoftware::uiEvents);

	// now to handle all events from all uis (shortcut, etc)
	ofAddListener(_ui->uiEvent, this, &ofxMicroUISoftware::uiEventsAll);
	for (auto & u : _ui->allUIs) {
		ofAddListener(u->uiEvent, this, &ofxMicroUISoftware::uiEventsAll);
	}

	_ui->load(_ui->presetsRootFolder + "/master.xml");
	
	for (auto & u : _ui->uis) {
		if (u.second.loadMode == ofxMicroUI::MASTER) {
			string f = _ui->presetsRootFolder + "/" + u.first + ".xml";
			u.second.load(f);
		}
	}
	updateFboRect();
	if (_ui->pString["presetsFolder"] == "") {
		ofxMicroUI::radio * r = _ui->getRadio("presetsFolder");
		if (r != NULL) {
			r->set("1");
		}
//            ((ofxMicroUI::radio*)_ui->getElement("presetsFolder"))->set("1");
	}
}

void ofxMicroUISoftware::addControlUI(ofxMicroUI * _ui) {
	ofAddListener(_ui->uiEvent, this, &ofxMicroUISoftware::uiEvents);
}


void ofxMicroUISoftware::setUI(ofxMicroUI * u) {
	cout << "ofxMicroUISoftware setUI! " << endl;
	_ui = u;
	afterSetUI();
	
	// podia checar se ja nao foi feito antes
	setup();
}

void ofxMicroUISoftware::updateFboRect() {
	fboRect = ofRectangle(_ui->pInt["fboX"],
		  _ui->pInt["fboY"],
		  fboFinal->getWidth() * _ui->pFloat["fboScale"],
		  fboFinal->getHeight() * _ui->pFloat["fboScale"]
	);
}


void ofxMicroUISoftware::drawFbo() {
	if (_ui != NULL) {
		if (showFbo) {
//			ofDisableDepthTest();
//			ofSetColor(0);
//			ofDrawRectangle(fboRect);
			ofSetColor(255);
			fboFinal->draw(fboRect);
		}
	}
}

void ofxMicroUISoftware::allocateFbos(int w, int h, int multiSampling) {
	cout << "allocateFbos : " << w << ":" << h << endl;
	if (multiSampling) {
		fbo.allocate(w, h, depth, multiSampling);
		fbo2.allocate(w, h, depth, multiSampling);
		fbo3.allocate(w, h, depth, multiSampling);
	} else {
		fbo.allocate(w, h, depth);
		fbo2.allocate(w, h, depth);
		fbo3.allocate(w, h, depth);
	}
	fboPixels.allocate(w, h, OF_IMAGE_COLOR); //OF_IMAGE_COLOR_ALPHA

	fbo.begin();
	ofClear(0,255);
	fbo.end();
	
	fbo2.begin();
	ofClear(0,255);
	fbo2.end();
	
	fbo3.begin();
	ofClear(0,255);
	fbo3.end();
}

void ofxMicroUISoftware::loadPreset(string s) {
	if (_ui->presetElement != NULL) {
		_ui->presetElement->set(s);
	}
}

void ofxMicroUISoftware::keyPressed(int key){
	if (key == OF_KEY_LEFT) {
		if (_ui->presetElement != NULL) {
			_ui->presetElement->cycle(-1);
		}
	}
	
	else if (key == OF_KEY_RIGHT) {
		if (_ui->presetElement != NULL) {
			_ui->presetElement->cycle(1);
		}
	}
	
	else if (key == OF_KEY_UP) {
		if (_ui->presetElement != NULL) {
			_ui->presetElement->cycle(-_ui->_settings->presetCols);
		}
	}
	
	else if (key == OF_KEY_DOWN) {
		if (_ui->presetElement != NULL) {
			_ui->presetElement->cycle(_ui->_settings->presetCols);
		}
	}

	// scrolling diogo
	else if (key == '[') {
		softScroll ++ ;
		if (softScroll > 0) {
			softScroll = 0;
		}
		_ui->_settings->offset.x = softScroll*softW;
	}
	else if (key == ']') {
		softScroll -- ;
		_ui->_settings->offset.x = softScroll*softW;
	}
	
	else if (key == '\\') {
		usingSoftScroll ^= 1;
		_ui->_settings->offset.x = usingSoftScroll ? softScroll*softW : 0;
	}
//		if (ofGetKeyPressed(OF_KEY_SUPER)) {
//			cout << "OF_KEY_SUPER" << endl;
//		}
//
//		else if (ofGetKeyPressed(OF_KEY_ALT)) {
//			cout << "OF_KEY_ALT" << endl;
//		}
//
//		else if (ofGetKeyPressed(OF_KEY_CONTROL)) {
//			cout << "OF_KEY_CONTROL" << endl;
//		}
//
//		else if (ofGetKeyPressed(OF_KEY_COMMAND)) {
//			cout << "OF_KEY_COMMAND" << endl;
//		}
//
//		else if (ofGetKeyPressed(OF_KEY_SHIFT)) {
//			cout << "OF_KEY_SHIFT" << endl;
//		}


	if (ofGetKeyPressed(OF_KEY_COMMAND)) {
		if (key == 'f' || key == 'F') {
			ofToggleFullscreen();
		}
		else if (key == 's' || key == 'S') {
			string name = _ui->pString["presets"];
			//cout << "saving actual preset " << name << endl;
			_ui->savePreset(name);
		}
		else if (key == 'o' || key == 'O') {
			string n = _ui->pString["presets"];
			string presetFolder = ofToDataPath(_ui->getPresetPath(true) + "/" + n);
			string comando = "open " + presetFolder;
			cout << comando << endl;
			ofSystem(comando);
		}
	} else {
		// no key command pressed
		
		if (key == '=') {
			_ui->toggleVisible();
			if (!_ui->visible) {
				ofHideCursor();
			} else {
				ofShowCursor();
			}
		}
		else if (key == '-') {
			ofToggleFullscreen();
		}
//			else if (key == '0') {
//				_ui->toggleVisible();
//				showFbo = _ui->_settings->visible;
////                cout << showFbo << endl;
//				if (showFbo) {
//					ofSetWindowShape(windowSize.x, windowSize.y);
//				} else {
//					ofSetWindowShape(100, 100);
//					ofSetWindowPosition(30,30);
//				}
//			}
		
		if (usePresetShortcut && key < 255) {
			if ( keyPreset.find(key) != keyPreset.end() ) {
				if (_ui != NULL && _ui->presetElement != NULL) {
					ofxMicroUI::element * e = _ui->presetElement;
					if (e != NULL && e->name != "") {
						((ofxMicroUI::presets*)e)->set(keyPreset[key]);
					} else {
		//				cout << "e not found! ):" << endl;
					}
				}
			}
		}
	}
}

void ofxMicroUISoftware::uiEventMaster(string & e) {
//        if (e == "createFromText") {
	if (e == "setup") {
		afterSetUI();
	}
}

void ofxMicroUISoftware::uiEventsAll(ofxMicroUI::element & e) {
	// shortcutUIEvent(e);
	if (e.name == "resetAll") {
		if (!e._settings->presetIsLoading) {
			cout << e.name << "::" << e._ui->uiName << endl;
			for (auto & ee : e._ui->elements) {
				// evita loop infinito
				if (ee->name != "resetAll") {
					ee->resetDefault();
				}
			}
		}
	}
	
	if (ofIsStringInString(e.name, "_shortcutPlus")) {
		if (!e._settings->presetIsLoading && *e.s != "") {
			vector <string> explode = ofSplitString(e.name, "_shortcutPlus");
			float val = ofToFloat(*e.s);
			e._ui->getSlider(explode[0])->add(val);
		}
	}
	
	else if (ofIsStringInString(e.name, "_shortcut")) {
		if (!e._settings->presetIsLoading && *e.s != "") {
			vector <string> explode = ofSplitString(e.name, "_shortcut");
			float val = ofToFloat(*e.s);
			e._ui->getSlider(explode[0])->set(val);
		}
	}
}

// Master only
void ofxMicroUISoftware::uiEvents(ofxMicroUI::element & e) {
	if (e.name == "easing") {
		_ui->_settings->easing = *e.f;
	}
	
	else if (e.name == "presetsFolder") {
		_ui->setPresetsFolder(*e.s);
	}

	else if (e.name == "fps") {
		
		// se o tipo for string.
		if (e.s != NULL) {
			
			cout << "FPS STRING " << *e.s << endl;
			ofSetFrameRate(ofToInt(*e.s));
		}
		if (e.i != NULL) {
//                cout << "FPS INT " << *e.i << endl;
//				cout << "ofxMicroUISoftware :: " << e.name << " :: " << *e.i << endl;
			ofSetFrameRate(*e.i);
		}
	}
	
	else if (e.name == "fboX" || e.name == "fboY" || e.name == "fboScale") {
		updateFboRect();
	}
	
	else if (e.name == "opacityUI") {
		_ui->_settings->uiOpacity = *e.f;
		_ui->uiOpacity = *e.f;
	}
	
	else if (e.name == "verticalSync") {
		cout << "ofxMicroUISoftware :: " << e.name << " :: " << *e.b << endl;
		ofSetVerticalSync(*e.b);
	}
	
	else if (e.name == "dragFbo") {
		dragFbo = *e.b;
	}
	
	if (e.tag == "showUIByName") {
		for (auto & u : _ui->uis) {
			if (ofIsStringInString(u.first, e.name)) {
				u.second.setVisible (*e.b);
			}
		}
		_ui->reflowUIs();
	}
	
	else if (e.tag == "showUI") {
		for (auto & u : _ui->allUIs) {
			if (u->uiTag == e.name) {
				u->setVisible (*e.b);
			}
		}
		_ui->reflowUIs();
	}
}


void ofxMicroUISoftware::onKeyPressed(ofKeyEventArgs & data) {
	keyPressed(data.key);
}

void ofxMicroUISoftware::onMousePressed(ofMouseEventArgs & data) {
	if (dragFbo) {
		glm::vec2 xy = glm::vec2(data.x, data.y);
		if (fboRect.inside(xy)) {
			firstXY = xy;
			dragging = true;
		}
	}
}

void ofxMicroUISoftware::onMouseDragged(ofMouseEventArgs & data) {
	if (dragFbo && dragging) {
		glm::vec2 xy = glm::vec2(data.x, data.y);
		fboRect.x += data.x - firstXY.x;
		fboRect.y += data.y - firstXY.y;
		firstXY = xy;
		float x = fboRect.x;
		float y = fboRect.y;
		_ui->getSlider("fboY")->set(y);
		_ui->getSlider("fboX")->set(x);
	}
}

void ofxMicroUISoftware::onMouseReleased(ofMouseEventArgs & data) {
	dragging = false;
}

void ofxMicroUISoftware::onExit(ofEventArgs & data) {
	if (_ui != NULL) {
		cout << "ofxMicroUISoftware exit, saving master.xml" << endl;
		//cout << _ui->presetsRootFolder << endl;
		_ui->save(_ui->presetsRootFolder + "/master.xml");
		for (auto & u : _ui->uis) {
			if (u.second.saveMode == ofxMicroUI::MASTER) {
				string f = _ui->presetsRootFolder + "/" + u.first + ".xml";
				u.second.save(f);
			}
		}
	}
	else {
		cout << "ofxMicroUISoftware need to set ui pointer" << endl;
	}
}


void ofxMicroUISoftware::fboToPixels(bool useShort) {
	if (!fboExport.isAllocated()) {
		cout << "allocating fboExport" << endl;
		fboExport.allocate(fboFinal->getWidth(), fboFinal->getHeight(), useShort ? depth : GL_RGB);
	}
	fboExport.begin();
	ofClear(0,255);
	ofSetColor(255);
	fboFinal->draw(0,0);
	fboExport.end();
	if (useShort) {
		fboExport.readToPixels(shortPixelsExport);
	} else {
		fboExport.readToPixels(pixelsExport);
	}
}

void ofxMicroUISoftware::fboToPng() {
	fboToPixels();
	string p = ofToString(_ui->pString["presets"]);
	string folder = "_output";
	if (!ofFile::doesFileExist(folder)) {
		ofDirectory::createDirectory(folder);
	}
	// create directory if doesnt exist
	string fullFileName = folder + "/" + p + "_" +ofGetTimestampString() + ".png";
	// ofSaveImage(pixelsExport, fullFileName);
	ofSaveImage(shortPixelsExport, fullFileName);
//	string resultado = ofSystem("open " + ofxMicroUI::dataPath(fullFileName));
	string resultado = ofSystem("open " + ofToDataPath(fullFileName));
}
