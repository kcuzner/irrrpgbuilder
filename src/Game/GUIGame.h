#ifndef GUIGAME_H
#define GUIGAME_H
#include "../gui/GUIManager.h"

//Used to create the GAUGE balls GUI in the GAME gui
#include "../gui/CGUIGfxStatus.h"

#include "../LANGManager.h"
#include "../sound/SoundManager.h"
#include <vector>

class GUIGame {
public:
	static GUIGame* getInstance();
	void setupGameplayGUI();
	void setCutsceneText(core::stringw text);
	void showCutsceneText(bool visible);
	void drawPlayerStats();
	void showBlackScreen(stringc text);
	void hideBlackScreen();

	void showDialogMessage(stringw text, std::string sound);
	bool showDialogQuestion(stringw text, std::string sound );
	stringc showInputQuestion(stringw text);
	void stopDialogSound();
	stringc getActivePlayerItem();
	void updateItemsList();
	DynamicObject* getActiveLootItem();


private:
	//private stuff
	GUIGame();
	~GUIGame();

	dimension2d<u32> screensize; //This need to be retrieved from the APP class, has the screen size info.
	s32 displayheight;
	s32 displaywidth;

	//Store the texture of the dialog buttons
	ITexture* guiDialogImgYes; 
    ITexture* guiDialogImgYes_s; 
    ITexture* guiDialogImgNo;
    ITexture* guiDialogImgNo_s;
	position2di mainToolbarPos;


	IGUIEnvironment* guienv;
	IVideoDriver* driver;

	//Require the nodepreview custom guiclass
	NodePreview* nodepreview; 

	

	// used for sounds in dialogs;
	ISound* dialogSound;

	u32 timer;
	u32 timer2;

	GUIConfigWindow* configWindow;
		



};




#endif