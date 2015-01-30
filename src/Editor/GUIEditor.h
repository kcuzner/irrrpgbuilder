#ifndef GUIEDITOR_H
#define GUIEDITOR_H

#include "../gui/GUIManager.h"
#include "../objects/DynamicObject.h"
#include "../LANGManager.h"
#include <vector>

class GUIEditor {
public:
	static GUIEditor* getInstance();

	void setupEditorGUI();
	void createDisplayOptionsGUI();
	void createMainToolbar();
	void createMainTabs();
	void createProjectTab();
	void createPlayTab();
	void createEnvironmentTab();
	void createObjectTab();

	void createAboutWindowGUI();

	void createTerrainToolbar();
	void createVegetationToolbar();
	void createDynamicObjectChooserGUI();

	void createContextMenuGUI();
	void createCodeEditorGUI();

	void drawHelpImage(u32 img);

	stringw getEditCameraString(ISceneNode* node);
	void updateEditCameraString(scene::ISceneNode * node); 

	f32 getScrollBarValue(u32 id);

	void UpdateGUIChooser(u32 type);
	void updateCurrentCategory(u32 type);
	void UpdateCollections(u32 type);
	void buildSceneObjectList(DynamicObject::TYPE objtype);

	void getInfoAboutModel(u32 type);

	void loadScriptTemplates();

	inline GUIConfigWindow* getConfigWindow() {return configWindow;}

	
	

private:
	//private stuff
	GUIEditor();
	~GUIEditor();

	dimension2d<u32> screensize; //This need to be retrieved from the APP class, has the screen size info.
	s32 displayheight;
	s32 displaywidth;
	IGUIEnvironment* guienv;
	IVideoDriver* driver;
	IrrlichtDevice *device;

	ITexture* info_current; // current assigned texture
	ITexture* info_current1; // current assigned texture inside the Custom Segments

	// Textures
	ITexture* backtexture;
	ITexture* imgNewProject;
	ITexture* imgNewProject1;
	ITexture* imgLoadProject;
	ITexture* imgLoadProject1;
	ITexture* imgSaveProject;
	ITexture* imgSaveProject1;
	ITexture* imgCloseProgram;
	ITexture* imgAbout;
	ITexture* imgAbout1;
	ITexture* imgHelp;
	ITexture* imgHelp1;
	ITexture* imgConfig;
	ITexture* imgConfig1;

	///IrrRPG Builder LOGO
    ITexture* logo1;

	ITexture* helpTerrainTransform;
    ITexture* helpTerrainSegments;
	ITexture* helpVegetationPaint;

	GUIConfigWindow* configWindow;

	position2di mainToolbarPos;

};




#endif
