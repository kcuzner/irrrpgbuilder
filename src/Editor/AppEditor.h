#ifndef APPEDITOR_H
#define APPEDITOR_H

#ifdef WIN32 //To get special folders on Windows
	#include <windows.h>
	#include <Shlobj.h>
	#include <iostream>
#endif

#include <irrlicht.h>
#include <vector>
#include <iostream>
#include <fstream>

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

class AppEditor {
public:
	AppEditor();
	~AppEditor();

	inline void setUserDocumentsPath(stringw path) { pathuserdocuments = path; }
	inline stringw getUserDocumentsPath() { return pathuserdocuments; }

	inline void setApplicationPath(stringw path) { pathapplication = path; }
	inline stringw getApplicationPath() { return pathapplication; }

	inline void setProjectsPath(stringw path) { pathprojects = path; }
	inline stringw getProjectsPath() { return pathprojects; }

	inline void setAppDataPath(stringw path) { pathappdata = path; }
	inline stringw getAppDataPath() { return pathappdata; }

	bool checkPath(stringc path);

	bool createFolder(stringw foldername);

	void copyConfiguration();

private:
	//private stuff
	core::stringw pathuserdocuments;
	core::stringw pathapplication;
	core::stringw pathprojects;
	core::stringw pathappdata;
};

#endif